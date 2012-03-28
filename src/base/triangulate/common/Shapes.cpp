/* 
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "Shapes.h"
#include <iostream>

namespace avg {

TriangulationTriangle::TriangulationTriangle(Point& a, Point& b, Point& c)
{
	m_points[0] = &a;
	m_points[1] = &b;
	m_points[2] = &c;
	m_neighbors[0] = NULL;
	m_neighbors[1] = NULL;
	m_neighbors[2] = NULL;
	m_constrained_edge[0] = m_constrained_edge[1] = m_constrained_edge[2] = false;
	m_delaunay_edge[0] = m_delaunay_edge[1] = m_delaunay_edge[2] = false;
	m_interior = false;
}

// Update neighbor pointers
void TriangulationTriangle::MarkNeighbor(Point* p1, Point* p2,
		TriangulationTriangle* t)
{
	if ((p1 == m_points[2] && p2 == m_points[1])
			|| (p1 == m_points[1] && p2 == m_points[2])) {
		m_neighbors[0] = t;
	} else if ((p1 == m_points[0] && p2 == m_points[2])
			|| (p1 == m_points[2] && p2 == m_points[0])) {
		m_neighbors[1] = t;
	} else if ((p1 == m_points[0] && p2 == m_points[1])
			|| (p1 == m_points[1] && p2 == m_points[0])) {
		m_neighbors[2] = t;
	} else {
		assert(0);
	}

}

// Exhaustive search to update neighbor pointers
void TriangulationTriangle::MarkNeighbor(TriangulationTriangle& t)
{
	if (t.Contains(m_points[1], m_points[2])) {
		m_neighbors[0] = &t;
		t.MarkNeighbor(m_points[1], m_points[2], this);
	} else if (t.Contains(m_points[0], m_points[2])) {
		m_neighbors[1] = &t;
		t.MarkNeighbor(m_points[0], m_points[2], this);
	} else if (t.Contains(m_points[0], m_points[1])) {
		m_neighbors[2] = &t;
		t.MarkNeighbor(m_points[0], m_points[1], this);
	}
}

/**
 * Clears all references to all other triangles and points
 */
void TriangulationTriangle::Clear()
{
	TriangulationTriangle *t;
	for (int i = 0; i < 3; i++) {
		t = m_neighbors[i];
		if (t != NULL) {
			t->ClearNeighbor(this);
		}
	}
	ClearNeighbors();
	m_points[0] = m_points[1] = m_points[2] = NULL;
}

void TriangulationTriangle::ClearNeighbor(TriangulationTriangle *triangle)
{
	if (m_neighbors[0] == triangle) {
		m_neighbors[0] = NULL;
	} else if (m_neighbors[1] == triangle) {
		m_neighbors[1] = NULL;
	} else {
		m_neighbors[2] = NULL;
	}
}

void TriangulationTriangle::ClearNeighbors()
{
	m_neighbors[0] = NULL;
	m_neighbors[1] = NULL;
	m_neighbors[2] = NULL;
}

void TriangulationTriangle::ClearDelunayEdges()
{
	m_delaunay_edge[0] = m_delaunay_edge[1] = m_delaunay_edge[2] = false;
}

Point* TriangulationTriangle::OppositePoint(TriangulationTriangle& t,
		Point& p) {
	Point *cw = t.PointCW(p);
	double x = cw->m_x;
	double y = cw->m_y;
	x = p.m_x;
	y = p.m_y;
	return PointCW(*cw);
}

// Legalized triangle by rotating clockwise around point(0)
void TriangulationTriangle::Legalize(Point& point)
{
	m_points[1] = m_points[0];
	m_points[0] = m_points[2];
	m_points[2] = &point;
}

// Legalize triagnle by rotating clockwise around oPoint
void TriangulationTriangle::Legalize(Point& opoint, Point& npoint)
{
	if (&opoint == m_points[0]) {
		m_points[1] = m_points[0];
		m_points[0] = m_points[2];
		m_points[2] = &npoint;
	} else if (&opoint == m_points[1]) {
		m_points[2] = m_points[1];
		m_points[1] = m_points[0];
		m_points[0] = &npoint;
	} else if (&opoint == m_points[2]) {
		m_points[0] = m_points[2];
		m_points[2] = m_points[1];
		m_points[1] = &npoint;
	} else {
		assert(0);
	}
}

unsigned int TriangulationTriangle::Index(const Point* p)
{
	if (p == m_points[0]) {
		return 0;
	} else if (p == m_points[1]) {
		return 1;
	} else if (p == m_points[2]) {
		return 2;
	}
	assert(0);
}

unsigned int TriangulationTriangle::EdgeIndex(const Point* p1,
		const Point* p2)
{
	if (m_points[0] == p1) {
		if (m_points[1] == p2) {
			return 2;
		} else if (m_points[2] == p2) {
			return 1;
		}
	} else if (m_points[1] == p1) {
		if (m_points[2] == p2) {
			return 0;
		} else if (m_points[0] == p2) {
			return 2;
		}
	} else if (m_points[2] == p1) {
		if (m_points[0] == p2) {
			return 1;
		} else if (m_points[1] == p2) {
			return 0;
		}
	}
	return -1;
}

void TriangulationTriangle::MarkConstrainedEdge(const int index)
{
	m_constrained_edge[index] = true;
}

void TriangulationTriangle::MarkConstrainedEdge(Edge& edge)
{
	MarkConstrainedEdge(edge.m_p, edge.m_q);
}

// Mark edge as constrained
void TriangulationTriangle::MarkConstrainedEdge(Point* p, Point* q)
{
	if ((q == m_points[0] && p == m_points[1])
			|| (q == m_points[1] && p == m_points[0])) {
		m_constrained_edge[2] = true;
	} else if ((q == m_points[0] && p == m_points[2])
			|| (q == m_points[2] && p == m_points[0])) {
		m_constrained_edge[1] = true;
	} else if ((q == m_points[1] && p == m_points[2])
			|| (q == m_points[2] && p == m_points[1])) {
		m_constrained_edge[0] = true;
	}
}

// The point counter-clockwise to given point
Point* TriangulationTriangle::PointCW(Point& point)
{
	if (&point == m_points[0]) {
		return m_points[2];
	} else if (&point == m_points[1]) {
		return m_points[0];
	} else if (&point == m_points[2]) {
		return m_points[1];
	}
	assert(0);
}

// The point counter-clockwise to given point
Point* TriangulationTriangle::PointCCW(Point& point)
{
	if (&point == m_points[0]) {
		return m_points[1];
	} else if (&point == m_points[1]) {
		return m_points[2];
	} else if (&point == m_points[2]) {
		return m_points[0];
	}
	assert(0);
}

// The neighbor clockwise to given point
TriangulationTriangle* TriangulationTriangle::NeighborCW(Point& point)
{
	if (&point == m_points[0]) {
		return m_neighbors[1];
	} else if (&point == m_points[1]) {
		return m_neighbors[2];
	}
	return m_neighbors[0];
}

// The neighbor counter-clockwise to given point
TriangulationTriangle* TriangulationTriangle::NeighborCCW(Point& point)
{
	if (&point == m_points[0]) {
		return m_neighbors[2];
	} else if (&point == m_points[1]) {
		return m_neighbors[0];
	}
	return m_neighbors[1];
}

bool TriangulationTriangle::GetConstrainedEdgeCCW(Point& p)
{
	if (&p == m_points[0]) {
		return m_constrained_edge[2];
	} else if (&p == m_points[1]) {
		return m_constrained_edge[0];
	}
	return m_constrained_edge[1];
}

bool TriangulationTriangle::GetConstrainedEdgeCW(Point& p)
{
	if (&p == m_points[0]) {
		return m_constrained_edge[1];
	} else if (&p == m_points[1]) {
		return m_constrained_edge[2];
	}
	return m_constrained_edge[0];
}

void TriangulationTriangle::SetConstrainedEdgeCCW(Point& p, bool ce)
{
	if (&p == m_points[0]) {
		m_constrained_edge[2] = ce;
	} else if (&p == m_points[1]) {
		m_constrained_edge[0] = ce;
	} else {
		m_constrained_edge[1] = ce;
	}
}

void TriangulationTriangle::SetConstrainedEdgeCW(Point& p, bool ce)
{
	if (&p == m_points[0]) {
		m_constrained_edge[1] = ce;
	} else if (&p == m_points[1]) {
		m_constrained_edge[2] = ce;
	} else {
		m_constrained_edge[0] = ce;
	}
}

bool TriangulationTriangle::GetDelunayEdgeCCW(Point& p)
{
	if (&p == m_points[0]) {
		return m_delaunay_edge[2];
	} else if (&p == m_points[1]) {
		return m_delaunay_edge[0];
	}
	return m_delaunay_edge[1];
}

bool TriangulationTriangle::GetDelunayEdgeCW(Point& p)
{
	if (&p == m_points[0]) {
		return m_delaunay_edge[1];
	} else if (&p == m_points[1]) {
		return m_delaunay_edge[2];
	}
	return m_delaunay_edge[0];
}

void TriangulationTriangle::SetDelunayEdgeCCW(Point& p, bool e)
{
	if (&p == m_points[0]) {
		m_delaunay_edge[2] = e;
	} else if (&p == m_points[1]) {
		m_delaunay_edge[0] = e;
	} else {
		m_delaunay_edge[1] = e;
	}
}

void TriangulationTriangle::SetDelunayEdgeCW(Point& p, bool e)
{
	if (&p == m_points[0]) {
		m_delaunay_edge[1] = e;
	} else if (&p == m_points[1]) {
		m_delaunay_edge[2] = e;
	} else {
		m_delaunay_edge[0] = e;
	}
}

// The neighbor across to given point
TriangulationTriangle& TriangulationTriangle::NeighborAcross(Point& opoint)
{
	if (&opoint == m_points[0]) {
		return *m_neighbors[0];
	} else if (&opoint == m_points[1]) {
		return *m_neighbors[1];
	}
	return *m_neighbors[2];
}

void TriangulationTriangle::DebugPrint()
{
	using namespace std;
	cout << m_points[0]->m_x << "," << m_points[0]->m_y << " ";
	cout << m_points[1]->m_x << "," << m_points[1]->m_y << " ";
	cout << m_points[2]->m_x << "," << m_points[2]->m_y << endl;
}

}

