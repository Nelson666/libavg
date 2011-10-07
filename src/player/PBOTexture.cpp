//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
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

#include "PBOTexture.h"

#include "../graphics/GLContext.h"
#include "../graphics/GLTexture.h"
#include "../graphics/PBO.h"
#include "../graphics/BmpTextureMover.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/ScopeTimer.h"
#include "../base/ObjectCounter.h"
#include "../base/StringHelper.h"

#include <iostream>
#include <string>

namespace avg {

using namespace std;
    
PBOTexture::PBOTexture(IntPoint size, PixelFormat pf, const MaterialInfo& material) 
{
    ObjectCounter::get()->incRef(&typeid(*this));
    m_pTex = GLTexturePtr(new GLTexture(size, pf, material.getUseMipmaps(),
            material.getTexWrapSMode(), material.getTexWrapTMode())); 
    m_pWriteMover = TextureMover::create(size, pf, GL_DYNAMIC_DRAW);
}

PBOTexture::~PBOTexture()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

BitmapPtr PBOTexture::lockBmp()
{
    return m_pWriteMover->lock();
}

void PBOTexture::unlockBmp(bool bMoveToTexture)
{
    m_pWriteMover->unlock();
    if (bMoveToTexture) {
        m_pWriteMover->moveToTexture(m_pTex);
    }
    m_pTex->generateMipmaps();
}

BitmapPtr PBOTexture::readbackBmp()
{
    if (!m_pReadMover) {
        OGLMemoryMode memoryMode = GLContext::getCurrent()->getMemoryModeSupported();
        if (memoryMode == MM_PBO) {
            m_pReadMover = TextureMoverPtr(new PBO(m_pTex->getGLSize(), m_pTex->getPF(), 
                    GL_DYNAMIC_READ));
        } else {
            m_pReadMover = m_pWriteMover;
        }
    }
    return m_pReadMover->moveTextureToBmp(m_pTex);
}

void PBOTexture::setTex(GLTexturePtr pTex)
{
    m_pTex = pTex;
}

void PBOTexture::activate(int textureUnit)
{
    m_pTex->activate(textureUnit);
}

const IntPoint& PBOTexture::getTextureSize() const
{
    return m_pTex->getGLSize();
}

}
