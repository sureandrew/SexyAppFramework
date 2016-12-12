#pragma warning(disable:4244) 
#pragma warning(disable:4018)

#include "GameOverEffect.h"
#include "Res.h"
#include "Board.h"
#include "SexyAppFramework/SexyAppBase.h"
#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Font.h"
#include "SexyAppFramework/Image.h"

using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameOverEffect::GameOverEffect()
{
	Init();	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameOverEffect::Init()
{
	mState = GameOverEffect::RED_FADE_IN;
	mAlpha = 0;
	mRed = 200;
	mRedHoldCount = 100;
	mRedChange = 4;
	mActive = false;
	mUpdateCnt = 0;
	mHue = 0;
	mCanInitFirstLevel = false;
	mFadeOut = false;
	mExplosion.clear();

	mText.clear();
	mText.push_back(Letter(_S("G")));
	mText.push_back(Letter(_S("A")));
	mText.push_back(Letter(_S("M")));
	mText.push_back(Letter(_S("E")));
	mText.push_back(Letter(_S(" O")));
	mText.push_back(Letter(_S("V")));
	mText.push_back(Letter(_S("E")));
	mText.push_back(Letter(_S("R")));

	mLines.clear();

	// Make as many lines as the screen is wide, and stagger them by a 
	// random time up to 2 seconds, with speeds between 5 and 10 pixels per update.
	for (int i = 0; i < gSexyAppBase->mWidth; i++)
		mLines.push_back(DrippyLine((Rand() % 5) + 5, i, Rand() % 200));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameOverEffect::Update(void)
{
	++mUpdateCnt;

	// HSL is an alternative to specifying an RGB color format.
	// Using HSL lets us easily do the hyper blinking crazy weird
	// flashing effect commonly found in old games, such as Robotron.
	// Below, we increment the value by 7 per update. The &0xFF is an 
	// easy way to clamp the value between 0 and 255 instead of having to
	// do a separate if (mHue > 255) mHue -= 255. This lets the value
	// rollover and keep cycling.
	mHue = (mHue + 7) & 0xFF;

	if (!mFadeOut)
	{
		// 10 times per second we create a new planet explosion animation and place it at a random
		// position within the grid's area.
		if (mUpdateCnt % 10 == 0)
		{
			Explosion e;
			e.mFrame = 0;
			e.mX = GRID_START_X + (Rand() % (GRID_END_X - GRID_START_X - IMAGE_ATOMIC_EXPLOSION->GetCelWidth()));
			e.mY = GRID_START_Y + (Rand() % (GRID_END_Y - GRID_START_Y - IMAGE_ATOMIC_EXPLOSION->GetCelHeight()));

			mExplosion.push_back(e);							
		}
		
		// Play a sound of a planet exploding ever 4th new planet. If we did this every single
		// planet, then 10 times per second you'd hear it, and trust me, it's really irritating.
		if ((mState == RED_FADE_IN) || (mState == RED_HOLD))
			if (mUpdateCnt % 40 == 0)
				gSexyAppBase->PlaySample(SOUND_EXPLOSION);

		// Update each explosion animation. When it's done, remove it.
		for (int i = 0; i < mExplosion.size(); i++)
		{
			Explosion* e = &mExplosion[i];

			// The animation runs at 20FPS, so every 5 updates step the frame.
			if (mUpdateCnt % 5 == 0)
			{
				if (++e->mFrame >+ IMAGE_ATOMIC_EXPLOSION->GetCelWidth())
				{
					mExplosion.erase(mExplosion.begin() + i);
					--i;
				}
			}
		}
	}

	if (mState == GameOverEffect::RED_FADE_IN)
	{
		// Slowly make the red effect become fully opaque to cover
		// the screen, while at the same time making the red value
		// pulse between 200 and 255.
		if (++mAlpha >= 255)
		{
			mState = GameOverEffect::RED_HOLD;
			mAlpha = 255;
		}
		else
			PulseRed();
	}
	else if (mState == GameOverEffect::RED_HOLD)
	{
		// Hold for 1 second, the red pulsing effect for dramatic fun
		if (--mRedHoldCount <= 0)
		{
			mState = GameOverEffect::SHOWING_LETTERS;
			mUpdateCnt = 0;
			gSexyAppBase->PlaySample(SOUND_GAME_OVER_TEXT);
		}
		else
			PulseRed();
	}
	else if (mState == GameOverEffect::SHOWING_LETTERS)
	{
		// Start showing the GAME OVER letters. Fade out that fully
		// red screen, but still let it pulse while we do so.
		if (mAlpha > 0)
		{
			PulseRed();
			mAlpha -= 2;
		}

		// Increase the red value on each letter by 2 per update.
		// Once one letter has a red value of 50, let the next letter
		// after it begin to fade in. Stop looping if a letter has less
		// than 50 for its red value.
		bool done = true;
		for (int i = 0; i < mText.size(); i++)
		{
			Letter* l = &mText[i];

			if (l->mRed < 254)
			{
				done = false;
				l->mRed += 2;
			}

			if (l->mRed < 50)
			{
				done = false;
				break;
			}
		}

		if (done)
			mState = GameOverEffect::FADING_LETTERS;
	}
	else if (mState == GameOverEffect::FADING_LETTERS)
	{
		// Now make the lettesr fade out, starting with
		// the last letter. When a letter's red value drops
		// below 205, allow the letter before it to also begin
		// decreasing.
		bool done = true;
		for (int i = (int)mText.size() - 1; i >= 0; i--)
		{
			Letter* l = &mText[i];

			if (l->mRed > 1)
			{
				done = false;
				l->mRed -= 2;
			}

			if (l->mRed > 205)
			{
				done = false;
				break;
			}
		}

		if (done)
		{
			mAlpha = 0;
			mState = GameOverEffect::SHOWING_STATS;
			gSexyAppBase->PlaySample(SOUND_GAME_OVER_STATS);
		}

	}
	else if ((mState == GameOverEffect::DRIP_OUT) && !mFadeOut)
	{
		// move the drippy lines, but we aren't fading the screen out yet.
		bool alldone = true;
		for (int i = 0; i < mLines.size(); i++)
		{
			// Once a line's starting delay has passed, it moves downward.
			// Once it reaches the bottom of the screen, it begins to then
			// move back upward. When all lines have moved upward, and thus the
			// whole screen is totally opaque, we can begin the fade out sequence.
			DrippyLine* dl = &mLines[i];
			if (dl->mDelay > 0)
			{
				alldone = false;
				--dl->mDelay;
			}
			else if (!dl->mDripUp)
			{
				alldone = false;
				dl->mHeight += dl->mSpeed;
				if (dl->mHeight >= gSexyAppBase->mHeight)
				{
					dl->mHeight = 0;
					dl->mSpeed = 10;
					dl->mDripUp = true;
				}
			}
			else if (dl->mDripUp && (dl->mHeight < gSexyAppBase->mHeight))
			{
				dl->mHeight += dl->mSpeed;
				if (dl->mHeight >= gSexyAppBase->mHeight)
					dl->mHeight = gSexyAppBase->mHeight;
				else
					alldone = false;
			}

		}

		if (alldone)
		{
			// Now that the screen is fully covered, allow the board to set up the first level
			mCanInitFirstLevel = true;
			mFadeOut = true;
			mAlpha = 255;
		}
	}
	else if ((mState == GameOverEffect::DRIP_OUT) && mFadeOut)
	{
		if (--mAlpha <= 0)
			mActive = false;
	}
}

///
//////////////////////////////////////////////////////////////////////////
void GameOverEffect::PulseRed()
{
	mRed += mRedChange;
	if (mRed >= 255)
	{
		mRed = 255;
		mRedChange = -mRedChange;
	}
	else if (mRed <= 200)
	{
		mRed = 200;
		mRedChange = -mRedChange;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameOverEffect::Draw(Graphics* g)
{
	// Draw explosions for every state except the final fade out
	if (!mFadeOut)
	{
		for (int i = 0; i < mExplosion.size(); i++)
		{
			Explosion* e = &mExplosion[i];
			g->DrawImageCel(IMAGE_ATOMIC_EXPLOSION, e->mX, e->mY, e->mFrame);
		}
	}

	if ((mState == GameOverEffect::RED_FADE_IN) || (mState == GameOverEffect::RED_HOLD))
	{
		g->SetColor(Color(mRed, 0, 0, mAlpha));
		g->FillRect(0, 0, gSexyAppBase->mWidth, gSexyAppBase->mHeight);
	}
	else if ((mState == GameOverEffect::SHOWING_LETTERS) || (mState == GameOverEffect::FADING_LETTERS))
	{
		g->SetFont(FONT_HUNGARR);
		int x = gSexyAppBase->mWidth / 2 - FONT_HUNGARR->StringWidth(_S("GAME OVER")) / 2;
		int y = gSexyAppBase->mHeight / 2;

		for (int i = 0; i < mText.size(); i++)
		{
			Letter* l = &mText[i];
			if (l->mRed > 0)
			{
				g->SetColor(Color(l->mRed, 0, 0));
				g->DrawString(l->mChar, x, y);
				x += FONT_HUNGARR->StringWidth(l->mChar);
			}
		}

		if (mAlpha > 0)
		{
			g->SetColor(Color(mRed, 0, 0, mAlpha));
			g->FillRect(0, 0, gSexyAppBase->mWidth, gSexyAppBase->mHeight);
		}
	}
	else if (mState == GameOverEffect::SHOWING_STATS)
	{		
		g->DrawImage(IMAGE_HUNGARR_LOGO, gSexyAppBase->mWidth / 2 - IMAGE_HUNGARR_LOGO->mWidth / 2, 10);

		g->SetFont(FONT_HUNGARR);
		g->SetColor(Color::White);
		int rightX = gSexyAppBase->mWidth / 2;
		int y = 100;
		SexyString s;
		int strWidth;

		s = _S("FINAL SCORE: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(255, 0, 0));
		g->DrawString(CommaSeperate(mStats.mScore), rightX, y);
		
		g->SetColor(Color::White);
		y += FONT_HUNGARR->GetHeight();
		s = _S("LEVEL REACHED: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(255, 0, 0));
		g->DrawString(StrFormat(_S("%d"), mStats.mLevel), rightX, y);

		g->SetColor(Color::White);
		y += FONT_HUNGARR->GetHeight();
		s = _S("TOTAL PLANETS EATEN: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(255, 255, 0));
		g->DrawString(StrFormat(_S("%d"), mStats.mNumPlanetsEaten), rightX, y);

		g->SetColor(Color::White);
		y += FONT_HUNGARR->GetHeight();
		s = _S("TOTAL PEOPLE EATEN: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(255, 255, 0));
		g->DrawString(CommaSeperate(mStats.mPopulationConsumed), rightX, y);

		g->SetColor(Color::White);
		y += FONT_HUNGARR->GetHeight();
		s = _S("TASTIEST PLANET: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(0, 255, 0));
		g->DrawString(mStats.mFavoritePlanet, rightX, y);


		g->SetColor(Color::White);
		y += FONT_HUNGARR->GetHeight();
		s = _S("EXPORT EATEN MOST: ");
		strWidth = FONT_HUNGARR->StringWidth(s);
		g->DrawString(s, rightX - strWidth, y);
		g->SetColor(Color(0, 255, 0));
		g->DrawString(mStats.mFavoriteExport, rightX, y);
	}
	else if (mState == GameOverEffect::DRIP_OUT)
	{
		if (!mFadeOut)
		{
			// This is how we convert an HSL value to an RGB value, which we have to
			// do to specify the color for the graphics object. The function HSLToRGB
			// takes as parameters: hue, saturation, luminance. We want to leave the
			// saturation at max and luminance at half for our particular example.
			// The returned value is ANDed with 0xFFFFFFFF to clamp the values for
			// the alpha, red, green, and blue to the valid region of 0 to 255. 
			Color hue = gSexyAppBase->HSLToRGB(mHue, 255, 128) & 0xFFFFFFFF;
			for (int i = 0; i < mLines.size(); i++)
			{
				DrippyLine* dl = &mLines[i];
				if (!dl->mDripUp && (dl->mDelay == 0))
				{
					// Draw a black line, with its bottommost pixel using the HSL color
					g->SetColor(Color(0, 0, 0));
					g->DrawRect(Rect(dl->mX, 0, 1, dl->mHeight - 1));
					g->SetColor(hue);
					g->DrawRect(Rect(dl->mX, dl->mHeight, 1, 1));
				}
				else if (dl->mDelay == 0)
				{
					// Moving up, draw the line in HSL and the rest in black
					if (dl->mHeight < gSexyAppBase->mHeight)
					{
						g->SetColor(Color(0, 0, 0));
						g->DrawRect(Rect(dl->mX, 0, 1, gSexyAppBase->mHeight - dl->mHeight));
					}

					g->SetColor(hue);
					g->DrawRect(Rect(dl->mX, gSexyAppBase->mHeight - dl->mHeight, 1, dl->mHeight));
				}
			}
		}
		else
		{
			// We AND the color with 0xFFFFFF instead of 0xFFFFFFFF. Then, we OR it with the alpha value
			// that we're fading out with shifted left 24 bits. 
			//	This then sticks the alpha value, which changes over time,
			// into our HSL color. As you may recall from previous demos, the actual color
			// structure is 32 bit, and looks like this in binary form:
			//	AAAA RRRR GGGG BBBB  Where A,R,G,B are alpha, red, green, blue.
			Color hue = (gSexyAppBase->HSLToRGB(mHue, 255, 128) & 0xFFFFFF) | (mAlpha << 24);
			g->SetColor(hue);
			g->FillRect(0, 0, gSexyAppBase->mWidth, gSexyAppBase->mHeight);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool GameOverEffect::CanInitFirstLevel(void)
{
	if (mCanInitFirstLevel)
	{
		mCanInitFirstLevel = false;		
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool GameOverEffect::HideBoard()
{
	return (mState == GameOverEffect::SHOWING_LETTERS) ||
			(mState == GameOverEffect::RED_HOLD) || 
			(mState == GameOverEffect::FADING_LETTERS) ||
			(mState == GameOverEffect::SHOWING_STATS) ||
			((mState == GameOverEffect::DRIP_OUT) && !mFadeOut);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameOverEffect::DoneViewingStats()
{
	mState = DRIP_OUT;
	gSexyAppBase->PlaySample(SOUND_GAME_OVER_CLICK);
	gSexyAppBase->PlaySample(SOUND_GAME_OVER_RESTART);
}