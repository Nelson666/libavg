//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2008 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "NullFXNode.h"
#include "SDLDisplayEngine.h"

#include "../base/ObjectCounter.h"

namespace avg {

NullFXNode::NullFXNode() 
    : FXNode()
{
    ObjectCounter::get()->incRef(&typeid(*this));
}

NullFXNode::~NullFXNode()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void NullFXNode::connect(SDLDisplayEngine* pEngine)
{
    FXNode::connect(pEngine);
}

void NullFXNode::disconnect()
{
    FXNode::disconnect();
}

void NullFXNode::setSize(const IntPoint& newSize)
{
    FXNode::setSize(newSize);
}

void NullFXNode::apply(GLTexturePtr pSrcTex)
{
    FBOPtr pFBO = getFBO();
    pSrcTex->activate();
    pFBO->activate();
    pFBO->setupImagingProjection();
    pFBO->drawImagingVertexes();
    pFBO->deactivate();
    pFBO->copyToDestTexture();
}

}