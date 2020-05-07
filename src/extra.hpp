#pragma once

#include <SDL.h>
#include <vector>

namespace hui {
	/**
	 *	\brief Calculate a point which is a given precentage along a line.
	 *
	 *	\param p1 The start point.
	 *	\param p2 The end point.
	 *	\param progress The precent distance from p1 to p2
	 *
	 *	\return The point which was calculated
	 */
	inline SDL_FPoint PointAlongLine(const SDL_FPoint p1, const SDL_FPoint p2, const float progress = 0.5);

	/**
	 *	\brief Calculate a point which is a given precentage along a Bezier curve.
	 *
	 *	\param p1 The start point.
	 *	\param p1c The start point's control point.
	 *	\param p2c The end point's control point.
	 *	\param p2 The end point.
	 *	\param progress The precent of distance along the curve to solve for
	 *
	 *	\returns The point which was calculated
	 */
	SDL_FPoint PointAlongBezierCurve(const SDL_FPoint p1, const SDL_FPoint p1c, const SDL_FPoint p2c, const SDL_FPoint p2, const float progress);

	/**
	*	\brief Render a Bezier curve on the target.
	*
	*	\param renderer The renderer to render the Bezier curve on.
	*	\param p1 The start point.
	*	\param p1c The start point's control point.
	*	\param p2c The end point's control point.
	*	\param p2 The end point.
	*	\param segments The number of segments which make up the Bezier curve.
	*	\param segmentsToRender The number of segments to draw. 0 to draw all.
	*
	*	\returns 0 on success, or -1 on error
	*/
	int RenderBezierCurve(SDL_Renderer* renderer, const SDL_FPoint p1, const SDL_FPoint p1c, const SDL_FPoint p2c, const SDL_FPoint p2, unsigned int segments = 16, unsigned int segmentsToRender = 0);
	
	/**
	 *	\/brief Render a Bezier curve on the target.
	 *
	 *	\/param renderer The renderer to render the Bezier curve on.
	 *	\/param points All points, in order, to draw the Bezier curve with.
	 *	\/param segments The number of segments which make up the Bezier curve.
	 *	\/param segmentsToRender The number of segments to render. 0 to draw all, >0 to render first X segments, <0 to render last X segments.
	 *
	 *	\/returns 0 on success, -1 on error
	 */
	//int RenderBezierCurve(SDL_Renderer* renderer, const std::vector<SDL_FPoint> points, unsigned int segments, unsigned int segmentsToDraw = 0);

	/**
	 *	\brief Rotate a point around a pivot point
	 *	
	 *	\param point The point to be rotated.
	 *	\param degrees How many degrees to rotate the point by.
	 *	\param pivot The point to rotate around.
	 *
	 *	\returns Translated point
	 */
	SDL_FPoint RotatePoint(SDL_FPoint point, double degrees, SDL_FPoint pivot = { 0.f, 0.f });
};