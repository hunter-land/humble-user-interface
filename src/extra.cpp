#include "extra.hpp"

#include <SDL.h>
#include <cmath> //sin and cos

namespace hui {
	SDL_FPoint PointAlongLine(const SDL_FPoint p1, const SDL_FPoint p2, const float progress) {
		return SDL_FPoint{
			p1.x*(1-progress) + p2.x*progress,
			p1.y*(1-progress) + p2.y*progress
		};
	}
	
	SDL_FPoint PointAlongBezierCurve(const SDL_FPoint p1, const SDL_FPoint p1c, const SDL_FPoint p2c, const SDL_FPoint p2, const float progress) {
		SDL_FPoint a, b, c, d, e, f;

		a = PointAlongLine(p1, p1c, progress);
		b = PointAlongLine(p1c, p2c, progress);
		c = PointAlongLine(p2c, p2, progress);

		d = PointAlongLine(a, b, progress);
		e = PointAlongLine(b, c, progress);

		f = PointAlongLine(d, e, progress);

		return f;
	}

	int RenderBezierCurve(SDL_Renderer* renderer, const SDL_FPoint p1, const SDL_FPoint p1c, const SDL_FPoint p2c, const SDL_FPoint p2, unsigned int segments, unsigned int segmentsToDraw) {
		if (segments < 3) {
			return -1; //Cannot create a curve with no segments, and 1 or 2 segments results in line
		}
		
		SDL_FPoint pPrev = p1; //Previous point is used for connecting the new point to the old, forming a line segment
		SDL_FPoint pCurr = p1; //Current point used for connecting with previous

		for (unsigned int i = 1; i <= segments && (i < segmentsToDraw || segmentsToDraw == 0); i++) {
			pPrev = pCurr;
			pCurr = PointAlongBezierCurve(p1, p1c, p2c, p2, (float)i/segments);
			SDL_RenderDrawLineF(renderer, pPrev.x, pPrev.y, pCurr.x, pCurr.y);
		}

		return 0;
	}


	SDL_FPoint RotatePoint(SDL_FPoint point, double degrees, SDL_FPoint pivot) {
		if (std::fmod(degrees, 360.) == 0) {
			return point;
		}
		SDL_FPoint returnPoint;
		double rad = degrees / 180 * M_PI;

		point.x -= pivot.x;
		point.y -= pivot.y;

		returnPoint.x = point.x*std::cos(rad) - point.y*std::sin(rad);
		returnPoint.y = point.y*std::cos(rad) + point.x*std::sin(rad);

		returnPoint.x += pivot.x;
		returnPoint.y += pivot.y;

		return returnPoint;
	}
};