//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2014 Ulrich von Zadow
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

#include "MCTexture.h"

#include "../base/Exception.h"
#include "../base/StringHelper.h"
#include "../base/MathHelper.h"
#include "../base/ObjectCounter.h"

#include "GLContext.h"
#include "GLContextManager.h"
#include "GLTexture.h"
#include "TextureMover.h"
#include "FBO.h"

#include <string.h>
#include <iostream>

namespace avg {

using namespace std;

MCTexture::MCTexture(const IntPoint& size, PixelFormat pf, bool bMipmap, bool bForcePOT,
        int potBorderColor)
    : TexInfo(size, pf, bMipmap, usePOT(bForcePOT, bMipmap), potBorderColor),
      m_bIsDirty(true)
{
    ObjectCounter::get()->incRef(&typeid(*this));
}

MCTexture::~MCTexture()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void MCTexture::initForGLContext()
{
    m_pTextures.push_back(GLTexturePtr(new GLTexture(*this)));
}

void MCTexture::activate(const WrapMode& wrapMode, int textureUnit)
{
    getCurTex()->activate(wrapMode, textureUnit);
}

void MCTexture::generateMipmaps()
{
    getCurTex()->generateMipmaps();
}

void MCTexture::moveBmpToTexture(BitmapPtr pBmp)
{
    getCurTex()->moveBmpToTexture(pBmp);
    m_bIsDirty = true;
}

BitmapPtr MCTexture::moveTextureToBmp(int mipmapLevel)
{
    return getCurTex()->moveTextureToBmp(mipmapLevel);
}

unsigned MCTexture::getID() const
{
    return getCurTex()->getID();
}

void MCTexture::setDirty()
{
    m_bIsDirty = true;
}

bool MCTexture::isDirty() const
{
    return m_bIsDirty;
}

void MCTexture::resetDirty()
{
    m_bIsDirty = false;
}

const GLTexturePtr& MCTexture::getCurTex() const
{
    return m_pTextures[GLContext::getMainIndex()];
}

}

