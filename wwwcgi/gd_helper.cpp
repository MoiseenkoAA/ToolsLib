
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/gd_helper.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains Linux classes for working with GD library.
 * The library implementation written
 * by Andrey A. Moiseenko (support@maasoftware.ru).
 * This library and applications are
 * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
 * as long as the following conditions are aheared to.
 *
 * Copyright remains Andrey A. Moiseenko, and as such any Copyright notices in
 * the code are not to be removed.  If this code is used in a product,
 * Andrey A. Moiseenko should be given attribution as the author of the parts used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Andrey A. Moiseenko (support@maasoftware.ru)
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREY A. MOISEENKO ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include "perm.h"
#include "temp.h"

#ifndef NO_GD

int CMaaGD_io_iface::__getC(struct gdIOCtx * ctx)
{
    return ((CMaaGD_io_iface *)ctx)->getC();
}
int CMaaGD_io_iface::__getBuf(struct gdIOCtx * ctx, void *ptr, int len)
{
    return ((CMaaGD_io_iface *)ctx)->getBuf(ptr, len);
}
void CMaaGD_io_iface::__putC(struct gdIOCtx * ctx, int ch)
{
    ((CMaaGD_io_iface *)ctx)->putC(ch);
}
int CMaaGD_io_iface::__putBuf(struct gdIOCtx * ctx, const void * ptr, int len)
{
    return ((CMaaGD_io_iface *)ctx)->putBuf(ptr, len);
}
int CMaaGD_io_iface::__seek(struct gdIOCtx * ctx, const int pos)
{
    return ((CMaaGD_io_iface *)ctx)->seek(pos);
}
long CMaaGD_io_iface::__tell(struct gdIOCtx * ctx)
{
    return ((CMaaGD_io_iface *)ctx)->tell();
}
void CMaaGD_io_iface::__gd_free(struct gdIOCtx * ctx)
{
    return ((CMaaGD_io_iface *)ctx)->gd_free();
}

CMaaGD_io_iface::CMaaGD_io_iface()
{
    gdIOCtx::getC = &__getC;
    gdIOCtx::getBuf = &__getBuf;
    gdIOCtx::putC = &__putC;
    gdIOCtx::putBuf = &__putBuf;
    gdIOCtx::seek = &__seek;
    gdIOCtx::tell = &__tell;
    gdIOCtx::gd_free = &__gd_free;
}

CMaaGD_io_iface::~CMaaGD_io_iface()
{
    /*
	gdIOCtx::getC = nullptr;
	gdIOCtx::getBuf = nullptr;
	gdIOCtx::putC = nullptr;
	gdIOCtx::putBuf = nullptr;
	gdIOCtx::seek = nullptr;
	gdIOCtx::tell = nullptr;
	gdIOCtx::gd_free = nullptr;
	*/
}
/*
    virtual int getC() = 0;
    virtual int getBuf(void *ptr, int len) = 0;
    virtual void putC(int ch) = 0;
    virtual int putBuf(const void * ptr, int len) = 0;
    // seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
    virtual int seek(const int pos) = 0;
    virtual long tell() = 0;
    virtual void gd_free() {};
*/


CMaaGD_io_MemoryCtx::CMaaGD_io_MemoryCtx(const CMaaString &srcmem) noexcept
//:   m_memory(0)
{
    m_Mode = 'r';
    m_limit = 0; // unused here
    m_srcmem = srcmem;
    m_pos = 0;
    m_size = m_srcmem.Length();
    //CMaaFile log(m_srcmem.Length() == 983094 ? "/var/ram/star/logs/adm.starominska.ru/bmp.txt" : nullptr, CMaaFile::eACD_SrSw, eNoExcept);
    //log.fprintf("constructor(), size=%d\n", (int)m_size);
}
CMaaGD_io_MemoryCtx::CMaaGD_io_MemoryCtx(int limit)
:   m_memory(1024)
{
    m_Mode = 'w';
    m_limit = limit;
    m_pos = 0;
    m_size = 0;
}
CMaaGD_io_MemoryCtx::~CMaaGD_io_MemoryCtx()
{
}

CMaaString CMaaGD_io_MemoryCtx::GetData()
{
    return m_Mode == 'w' ? CMaaString(m_memory, m_size) : m_srcmem;
}

CMaaString CMaaGD_io_MemoryCtx::GetStatistics()
{
    CMaaString tmp;
    tmp.Format("%c %d, limit %d, m_pos %d, m_size %d", m_Mode, m_srcmem.Length(), m_limit, m_pos, m_size);
    return tmp;
}

int CMaaGD_io_MemoryCtx::getC()
{
    if  (m_Mode == 'r')
    {
        //CMaaFile log(m_srcmem.Length() == 983094 ? "/var/ram/star/logs/adm.starominska.ru/bmp.txt" : nullptr, CMaaFile::eACD_SrSw, eNoExcept);
        //log.fprintf("getC(), pos=%d\n", (int)m_pos);
        if  (m_pos < m_srcmem.Length())
        {
            int x = (unsigned char)m_srcmem[m_pos++];
            //log.fprintf("getC(), ret=%d, pos=%d\n", x, (int)m_pos);
            return x;
        }
        return -1;
    }
    return -1;
}
int CMaaGD_io_MemoryCtx::getBuf(void *ptr, int len)
{
    if  (m_Mode == 'r')
    {
        //CMaaFile log(m_srcmem.Length() == 983094 ? "/var/ram/star/logs/adm.starominska.ru/bmp.txt" : nullptr, CMaaFile::eACD_SrSw, eNoExcept);
        //log.fprintf("getBuf(,%d), pos=%d\n", len, (int)m_pos);
        if  (len < 0)
        {
            len = 0;
        }
        if  (len > m_srcmem.Length() - m_pos)
        {
            len = m_srcmem.Length() - m_pos;
            //            return (unsigned char)m_srcmem[m_pos++];
        }
        memcpy(ptr, m_pos + (const char *)m_srcmem, len);
        m_pos += len;
        //log.fprintf("getBuf(,%d), pos=%d, ret %d\n", len, (int)m_pos, len > 0 ? len : -1);
        return len > 0 ? len : -1;
    }
    return -1;
}

void CMaaGD_io_MemoryCtx::putC(int ch)
{
    if  (m_Mode == 'w')
    {
        if  (m_pos < m_limit || m_limit == -1)
        {
            m_memory[m_pos++] = ch;
        }
    }
    m_size = m_size < m_pos ? m_pos : m_size;
}
int CMaaGD_io_MemoryCtx::putBuf(const void * ptr, int len)
{
    if  (m_Mode == 'w')
    {
        if  (len >= 0 && (len <= m_limit - m_pos || m_limit == -1))
        {
            m_memory[m_pos + len];
            memcpy(&m_memory[m_pos], ptr, len);
            m_pos += len;
        }
        else if (len >= 0 && m_limit - m_pos > 0)
        {
            len = m_limit - m_pos;
            m_memory[m_pos + len];
            memcpy(&m_memory[m_pos], ptr, len);
            m_pos += len;
        }
        else
        {
            len = -1;
        }
    }
    else
    {
        len = -1;
    }
    m_size = m_size < m_pos ? m_pos : m_size;
    return len;
}

// seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
int CMaaGD_io_MemoryCtx::seek(const int pos)
{
    if  (m_Mode == 'r')
    {
        //CMaaFile log(m_srcmem.Length() == 983094 ? "/var/ram/star/logs/adm.starominska.ru/bmp.txt" : nullptr, CMaaFile::eACD_SrSw, eNoExcept);
        //log.fprintf("seek(%d)", pos);
        if  (pos <= m_size && pos >= 0)
        {
            m_pos = pos;
            return 1;
        }
        return 0;
    }
    if  ((pos <= m_limit || m_limit == -1) && pos >= 0)
    {
        if  (pos > m_size)
        {
            m_memory[pos];
            memset(&m_memory[m_size], 0, pos - m_size);
        }
        m_pos = pos;
        return 1;
    }
    return 0;
}

long CMaaGD_io_MemoryCtx::tell()
{
    return m_pos;
}

void CMaaGD_io_MemoryCtx::gd_free()
{
}

//----------------------------------------------------
//class gd_Image //Helper
//----------------------------------------------------

#define img im

const char * gd_Image::GetTextSize(CMaaString Text, int * pSizeX, int * pSizeY, int * brect, CMaaString str_ttf_font_path, double ptsize, double angle)
{
    char *s = (char *)(const char *)Text;
    char * ttf_font_path = (char *)(const char *)str_ttf_font_path;

    int brect_[8];
    brect = brect ? brect : brect_;
    // obtain brect so that we can size the image (calcrect)
    const char * err = gdImageStringFT(nullptr, &brect[0], 0, ttf_font_path, ptsize, angle, 0, 0, s);
    int sx, sy;
    if  (err)
    {
        // error
        CMaaFile f_log("/var/ram/maa/ttf_log.txt", CMaaFile::eACD_SrSw, eNoExcept);
        f_log.fprintf("gd_Image::GetTextSize(): gdImageStringFT(): %s\n", err);
        //fprintf(stderr, "%s\n", err);
        sx = sy = 0;
    }
    else
    {
        // create an image big enough for the string plus a little whitespace
        sx = brect[2]-brect[6];
        sy = brect[3]-brect[7];
    }

    if  (pSizeX)
    {
        *pSizeX = sx;
    }
    if  (pSizeY)
    {
        *pSizeY = sy;
    }
    return err;
}

const char * gd_Image::DrawText(CMaaString Text, int x, int y, int Color, int * brect, CMaaString str_ttf_font_path, double ptsize, double angle)
{
    if  (!im)
    {
        return "im == nullptr (image is not created)";
    }

    const char * err;
    int borderrect[8];
    if  (!brect)
    {
        brect = borderrect;
        int sx, sy;
        err = GetTextSize(Text, &sx, &sy, brect, str_ttf_font_path, ptsize, angle);
        if  (err)
        {
            return err;
        }
    }

    char *s = (char *)(const char *)Text;
    char * ttf_font_path = (char *)(const char *)str_ttf_font_path;

    //err = gdImageStringFT(im, &brect[0], TextColor, ttf_font_path, ptsize, angle, (im->sx - sx) / 2 - brect[6], (im->sy - sy) / 2 - brect[7], s);
    //static CMaaString txt_;
    //txt_.Format("x - brect[6] == %d, y - brect[7] == %d", x - brect[6], y - brect[7]);
    err = gdImageStringFT(im, &brect[0], Color == -1 ? TextColor : Color, ttf_font_path, ptsize, angle, x - brect[6], y - brect[7], s);
    if  (err)
    {
        CMaaFile f_log("/var/ram/maa/ttf_log.txt", CMaaFile::eACD_SrSw, eNoExcept);
        f_log.fprintf("gd_Image::DrawText(): gdImageStringFT(): %s\n", err);
    }
    return err;// ? err : (const char *)txt_;
}

bool gd_Image::GetFixedFontTextSize(CMaaString Text, gdFontPtr pFont, int * pSizeX, int * pSizeY)
{
    int sx, sy;
    //pFont = pFont ? pFont : pSmallFont;
    if  (!pFont)
    {
        sx = sy = 0;
    }
    else
    {
        sx = Text.Length() * pFont->w;
        sy = pFont->h; // lines count testing
    }
    if  (pSizeX)
    {
        *pSizeX = sx;
    }
    if  (pSizeY)
    {
        *pSizeY = sy;
    }
    return pFont ? true : false;
}

//CMaaStringG g_uText;
bool gd_Image::DrawText(CMaaString Text, gdFontPtr pFont, int x, int y, int Color)
{
    if  (!im)
    {
        return false;
    }
    //pFont = pFont ? pFont : pSmallFont;
    if  (!pFont)
    {
        return false;
    }
    int n1 = Text.Utf8CharCount();
    if  (n1 < Text.Length())
    {
        CMaaString uText = Utf8ToUnicode(Text, 1251);
        uText = UnicodeToUnicodeBigEndian(uText);
        //g_uText = uText;
        gdImageString16(im, pFont, x, y, (unsigned short *)(const char *)uText, Color == -1 ? TextColor : Color);
    }
    else
    {
        gdImageString(im, pFont, x, y, (unsigned char *)(const char *)Text, Color == -1 ? TextColor : Color);
    }
    return true;
}

gd_Image::gd_Image()
{
    im = nullptr;

    m_LastX = 0, m_LastY = 0, m_LastColor = 0;

    BackgroundColor = -1;
    white = -1;
    black = -1;
    TextColor = -1;

    pTinyFont = gdFontGetTiny();
    pSmallFont = gdFontGetSmall();
    pMediumBoldFont = gdFontGetMediumBold();
    pLargeFont = gdFontGetLarge();
    pGiantFont = gdFontGetGiant();
}

gd_Image::~gd_Image()
{
    Destroy();
}

bool gd_Image::GetImageSize(int * pSizeX, int * pSizeY, bool bResetToZeroOnError)
{
    if  (im)
    {
        if  (pSizeX)
        {
            *pSizeX = im->sx;
        }
        if  (pSizeY)
        {
            *pSizeY = im->sy;
        }
        return true;
    }
    if  (bResetToZeroOnError)
    {
        if  (pSizeX)
        {
            *pSizeX = 0;
        }
        if  (pSizeY)
        {
            *pSizeY = 0;
        }
    }
    return false;
}

bool gd_Image::GetSize(int * pSizeX, int * pSizeY, bool bResetToZeroOnError)
{
    return GetImageSize(pSizeX, pSizeY, bResetToZeroOnError);
}

bool gd_Image::AcquireColorsBasic()
{
    if  (im)
    {
        BackgroundColor = BackgroundColor == -1 ? gdImageColorExact/*gdImageColorAllocate*/(im, 0, 195, 130) : BackgroundColor;
        white = white == -1 ? gdImageColorAllocate(im, 255, 255, 255) : white;
        black = black == -1 ? gdImageColorAllocate(im, 0, 0, 0) : black;
        TextColor = TextColor == -1 ? gdImageColorAllocate(im, 120, 255, 210) : TextColor; // RGB
        //TextColor2 = gdImageColorAllocate(im, 210, 255, 120); // RGB
        //{CMaaFile f("/var/ram/log__clr.txt", CMaaFile::eWCD_SrSw, eNoExcept); f.fprintf("%d %d\n", TextColor, BackgroundColor); }
        return true;
    }
    return false;
}

bool gd_Image::Create(int sx, int sy, bool bTransparent)
{
    if  (im)
    {
        return false;
    }
    im = gdImageCreateTrueColor(sx, sy);
    if  (!im)
    {
        return false;
    }
    AcquireColorsBasic();
    gdImageFilledRectangle(im, 0, 0, im->sx, im->sy, BackgroundColor);
    if  (BackgroundColor != -1 && bTransparent)
    {
        gdImageColorTransparent(im, BackgroundColor);
    }
    return true;
}

void gd_Image::FillRect(int x, int y, int sx, int sy, int Color)
{
    if  (!im)
    {
        return;
    }
    gdImageFilledRectangle(im, x, y, x + sx, y + sy, Color);
}

void gd_Image::SetTransparentColor(int Color)
{
    if  (!im && Color != -1)
    {
        gdImageColorTransparent(im, Color);
    }
}

bool gd_Image::IsValid()
{
    return im != nullptr;
}

bool gd_Image::Destroy()
{
    if  (im)
    {
        gdImageDestroy(im); im = nullptr;
    }
    return true;
}

int gd_Image::GenerateImage(int Type, CMaaString &Data, CMaaString &ContentType, int JpegQuality)
{
    if  (!im)
    {
        return eError;
    }

    CMaaGD_io_MemoryCtx __out;
    gdIOCtx * _out = (gdIOCtx *)&__out;

    switch(Type)
    {
    case ePng:
        gdImagePngCtx(im, _out);
        ContentType = "image/png";
        break;
    case eGif:
        gdImageGifCtx(im, _out);
        ContentType = "image/gif";
        break;
    case eBmp:
        //gdImageBmpCtx(im, gdImageColorAllocate(im, 0, 0, 0), _out, 1);
        gdImageBmpCtx(im, _out, 1);
        ContentType = "image/bmp";
        break;
        /*
	case ePng:
	    gdImagePngCtx(im, _out);
	    ContentType = "image/png";
	    break;
*/
    case eJpeg:
        JpegQuality = JpegQuality < 0 ? 70 : JpegQuality;
        gdImageJpegCtx(im, _out, JpegQuality);
        ContentType = "image/jpeg";
        break;
    default:
        return eError;
    }
    Data = __out.GetData();
    return Type;
}

bool gd_Image::Load(CMaaString FileName, bool bThrow, int Type, bool bAutoDestroy)
{
    if  (im)
    {
        if  (bAutoDestroy)
        {
            Destroy();
        }
        else
        {
            return false;
        }
    }
    //CMaaString ext = FileName.Extention();
    Type = Type < 0 ? GetTypeByFileNameExtention(FileName) : Type;

    switch(Type)
    {
    case ePng:
    case eGif:
    case eBmp:
    case eJpeg:
        break;
    default:
        if  (bThrow)
        {
            XTOOFile2Error err("unknown type error", 0);
            throw err;
        }
        return false;
    }

    CMaaFile fin(FileName, CMaaFile::eR_SrSw, bThrow);
    if  (!fin.IsOpen())
    {
        return false;
    }
    CMaaString src = fin.Read(100 * 1024 * 1024);
    fin.Close();

    CMaaGD_io_MemoryCtx __in(src);
    gdIOCtx * _in = (gdIOCtx *)&__in;

    switch(Type)
    {
    case ePng:
        im = gdImageCreateFromPngCtx(_in);
        break;
    case eGif:
        im = gdImageCreateFromGifCtx(_in);
        break;
    case eBmp:
        im = gdImageCreateFromBmpCtx(_in);
        break;
    case eJpeg:
        im = gdImageCreateFromJpegCtx(_in);
        break;
    }
    if  (!im)
    {
        if  (bThrow)
        {
            CMaaString xxx = __in.GetStatistics();
            CMaaString errtext;
            errtext.Format("create image from png/gif/bmp/jpeg source error %S, Type=%d, src.Length()=%d, %S", &FileName, Type, src.Length(), &xxx);
            XTOOFile2Error err(errtext, 0);
            throw err;
        }
    }
    AcquireColorsBasic();
    return im ? true : false;
}

bool gd_Image::LoadFromMemory(CMaaString Data, CMaaString Ext, bool bThrow, int Type, bool bAutoDestroy)
{
    if  (im)
    {
        if  (bAutoDestroy)
        {
            Destroy();
        }
        else
        {
            return false;
        }
    }
    //CMaaString ext = FileName.Extention();
    Type = Type < 0 ? GetTypeByFileNameExtention(Ext) : Type;

    switch(Type)
    {
    case ePng:
    case eGif:
    case eBmp:
    case eJpeg:
        break;
    default:
        if  (bThrow)
        {
            XTOOFile2Error err("unknown type error", 0);
            throw err;
        }
        return false;
    }

    CMaaGD_io_MemoryCtx __in(Data);
    gdIOCtx * _in = (gdIOCtx *)&__in;

    switch(Type)
    {
    case ePng:
        im = gdImageCreateFromPngCtx(_in);
        break;
    case eGif:
        im = gdImageCreateFromGifCtx(_in);
        break;
    case eBmp:
        im = gdImageCreateFromBmpCtx(_in);
        break;
    case eJpeg:
        im = gdImageCreateFromJpegCtx(_in);
        break;
    }
    if  (!im)
    {
        if  (bThrow)
        {
            CMaaString xxx = __in.GetStatistics();
            CMaaString errtext;
            errtext.Format("create image from png/gif/bmp/jpeg memory source error %S, Type=%d, Data.Length()=%d, %S", &Ext, Type, Data.Length(), &xxx);
            XTOOFile2Error err(errtext, 0);
            throw err;
        }
    }
    AcquireColorsBasic();
    return im ? true : false;
}

bool gd_Image::Save(CMaaString FileName, CMaaFile::eMode Mode, bool bThrow, int Type, int JpegQuality)
{
    if  (!im)
    {
        return false;
    }
    Type = Type < 0 ? GetTypeByFileNameExtention(FileName) : Type;
    CMaaString Data;
    CMaaString ContentType;
    if  (GenerateImage(Type, Data, ContentType, JpegQuality) == Type)
    {
        CMaaFile f(FileName, Mode, bThrow);
        if  (f.IsOpen() && f.Write(Data, Data.Length()) == (_dword)Data.Length())
        {
            return true;
        }
    }
    return false;
}

CMaaString gd_Image::GetInMemory(CMaaString Ext, bool bThrow, int Type, int JpegQuality)
{
    CMaaString Data;
    if  (!im)
    {
        return Data;
    }
    Type = Type < 0 ? GetTypeByFileNameExtention(Ext) : Type;
    CMaaString ContentType;
    if  (GenerateImage(Type, Data, ContentType, JpegQuality) == Type)
    {
        //return Data;
    }
    return Data;
}

int gd_Image::GetTypeByExtention(CMaaString ext)
{
    if  (ext[0] == '.')
    {
        ext = ext.RefMid(1);
    }
    ext = ext.Left(4).ToLower(0);

    if  (ext == "png")
    {
        return ePng;
    }
    if  (ext == "gif")
    {
        return eGif;
    }
    if  (ext == "bmp")
    {
        return eBmp;
    }
    if  (ext == "jpg" || ext == "jpeg" || ext == "jp")
    {
        return eJpeg;
    }
    return eError;
}

int gd_Image::GetTypeByFileNameExtention(CMaaString FileName)
{
    return GetTypeByExtention(FileName.Extention(0));
}

CMaaString gd_Image::GetExtentionByType(int Type, int dot)
{
    CMaaString Ret;
    switch(Type)
    {
    case ePng:
        Ret = "png";
        break;
    case eGif:
        Ret = "gif";
        break;
    case eBmp:
        Ret = "bmp";
        break;
    case eJpeg:
        Ret = "jpeg";
        break;
    default:
        return Ret;
    }
    if  (dot)
    {
        Ret = CMaaString(".") + Ret;
    }
    return Ret;
}

int gd_Image::GetDefaultColor(int defaultcolor)
{
    int clr = -1;
    switch(defaultcolor)
    {
    case eTextColor:
        clr = TextColor;
        break;
    case eWhiteColor:
        clr = white;
        break;
    case eBlackColor:
        clr = black;
        break;
    case eBackgroundColor:
        clr = BackgroundColor;
        break;
    }
    return clr;
}

int gd_Image::GetColorExact(int rgb, int defaultcolor)
{
    //{CMaaFile f("/var/ram/log__clr.txt", CMaaFile::eACD_SrSw, eNoExcept); f.fprintf("GetColorExact(%d %d)\n", rgb, defaultcolor); }
    rgb = (rgb & 0xffffff) == rgb && img ?
    gdImageColorExact(im, (rgb >> 16), (rgb >> 8) & 0xff, rgb & 0xff) :
    img ?
    GetDefaultColor(defaultcolor) :
    -1;
    return rgb;
}

int gd_Image::GetColor(int rgb, int defaultcolor)
{
    rgb = (rgb & 0xffffff) == rgb ? gdImageColorAllocate(im, (rgb >> 16), (rgb >> 8) & 0xff, rgb & 0xff) : GetDefaultColor(defaultcolor);
    return rgb;
}

int gd_Image::GetColorExact(int r, int g, int b, int defaultcolor)
{
    return GetColorExact( (((r|g|b) & 0xff) == (r|g|b)) ? ((r << 16) | (g << 8) | b) : -1, defaultcolor);
}

int gd_Image::GetColor(int r, int g, int b, int defaultcolor)
{
    return GetColor( (((r|g|b) & 0xff) == (r|g|b)) ? ((r << 16) | (g << 8) | b) : -1, defaultcolor);
}

int gd_Image::GetPixel(int x, int y)
{
    if  (IsValid())
    {
        return gdImageGetPixel(img, x, y);
    }
    return -1;
}

bool gd_Image::CopyResized(gd_Image &SrcImage, int srcX, int srcY, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH, bool bAutoAddTransparentColor)
{
    if  (IsValid() && SrcImage.IsValid())
    {
        if  (bAutoAddTransparentColor)
        {
            /*int TransparentColor = gdImageGetTransparent(SrcImage.img);
            //CMaaFile f("/var/ram/log_gd.txt", CMaaFile::eAC_SrSw, eNoExcept);
            //f.fprintf("TransparentColor=%06x\n", TransparentColor);
            if   (TransparentColor >= 0)
            {
                SetTransparentColor(TransparentColor);
            }
            //f.fprintf("TransparentColor=%06x\n\n", gdImageGetTransparent(img));
            */
            gdImageAlphaBlending(img, false);
            gdImageSaveAlpha(img, true);
        }
        gdImageCopyResampled(img, SrcImage.img, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
        return true;
    }
    return false;
}

bool gd_Image::CopyResized(gd_Image &SrcImage, bool bAutoAddTransparentColor)
{
    if  (IsValid() && SrcImage.IsValid())
    {
        int sx, sy;
        int insx, insy;
        GetSize(&sx, &sy);
        SrcImage.GetSize(&insx, &insy);
        return CopyResized(SrcImage, 0, 0, insx, insy, 0, 0, sx, sy, bAutoAddTransparentColor);
    }
    return false;
}

bool gd_Image::Copy(gd_Image &SrcImage, int srcX, int srcY, int W, int H, int dstX, int dstY, bool bAutoAddTransparentColor)
{
    if  (IsValid() && SrcImage.IsValid())
    {
        if  (bAutoAddTransparentColor)
        {
            /*int TransparentColor = gdImageGetTransparent(SrcImage.img);
            if   (TransparentColor >= 0)
            {
                SetTransparentColor(TransparentColor);
            }*/
            gdImageAlphaBlending(img, false);
            gdImageSaveAlpha(img, true);
        }
        gdImageCopy(img, SrcImage.img, dstX, dstY, srcX, srcY, W, H);
        return true;
    }
    return false;
}


bool gd_Image::Rotate(int angle, bool bAutoAddTransparentColor)
{
    int sx, sy;
    GetSize(&sx, &sy);
    gd_Image temp;
    if  (angle == -90 || angle == 90)
    {
        temp.Create(sy, sx);
    }
    else if (angle == 180 || angle == -180)
    {
        temp.Create(sx, sy);
    }
    else
    {
        return false;
    }
    int dsx, dsy;
    temp.GetSize(&dsx, &dsy);

    if  (bAutoAddTransparentColor)
    {
        gdImageAlphaBlending(temp.img, false);
        gdImageSaveAlpha(temp.img, true);
    }
    gdImageCopyRotated(temp.img, img, dsx / 2, dsy / 2, 0, 0, sx, sy, angle);
    Swap(temp);
    return true;
}

#define swp(t,a,b) {t tmp = a; a = b; b = tmp;}

void gd_Image::Swap(gd_Image &That)
{
    swp(gdImagePtr, img, That.img);
    swp(int, BackgroundColor, That.BackgroundColor);
    swp(int, white, That.white);
    swp(int, black, That.black);
    swp(int, TextColor, That.TextColor);
    swp(int, m_LastColor, That.m_LastColor);
    swp(int, m_LastX, That.m_LastX);
    swp(int, m_LastY, That.m_LastY);
}

void gd_Image::Line(int x1, int y1, int x2, int y2, int color)
{
    color = color == -1 ? m_LastColor : color;
    gdImageLine(im, x1, y1, x2, y2, m_LastColor = color);
    m_LastX = x2, m_LastY = y2;
}

void gd_Image::MoveTo(int x, int y)
{
    m_LastX = x, m_LastY = y;
}

void gd_Image::LineTo(int x2, int y2, int color)
{
    color = color == -1 ? m_LastColor : color;
    gdImageLine(im, m_LastX, m_LastY, x2, y2, m_LastColor = color);
    m_LastX = x2, m_LastY = y2;
}

void gd_Image::Arc(int centx, int centy, int w, int h, int start, int end, int color)
{
    color = color == -1 ? m_LastColor : color;
    gdImageArc(im, centx, centy, w, h, start, end, m_LastColor = color);
}

const char * gd_Image::DrawText2(CMaaString Text, int x, int y, int Color, double ptsize, CMaaString str_ttf_font_path)
{
    return DrawText(Text, x, y, Color, nullptr, str_ttf_font_path, ptsize);
}

const char * gd_Image::GetTextSize2(CMaaString Text, int * pSizeX, int * pSizeY, double ptsize, CMaaString str_ttf_font_path)
{
    return GetTextSize(Text, pSizeX, pSizeY, nullptr, str_ttf_font_path, ptsize);
}
#endif // #ifndef NO_GD

//using namespace CMaaHSVRGBcolor;

/*
constexpr float modulo(float dividend, float divisor)
{
    const auto quotient = dividend / divisor;
    //return divisor * (quotient - static_cast<uint8_t>(quotient));
    return dividend - divisor * static_cast<uint8_t>(quotient);
}
constexpr _byte max2(_byte a, _byte b)
{
    return a > b ? a : b;
}
constexpr _byte min2(_byte a, _byte b)
{
    return a < b ? a : b;
}
constexpr _byte max3(_byte a, _byte b, _byte c)
{
    return max2(max2(a, b), max2(a, c));
}
constexpr _byte min3(_byte a, _byte b, _byte c)
{
    return min2(min2(a, b), min2(a, c));
}
*/

void CMaaHSVRGBcolor::RGB2HSV(_byte r, _byte g, _byte b, float& H, float& S, float& V)
{
    const _byte c_max = max3(r, g, b);
    V = c_max / 255.0f;

    const _byte c_min = min3(r, g, b);
    const float delta = V - c_min / 255.0f;
    S = c_max == 0 ? 0 : delta / V;
    if  (c_max == c_min)
    {
        H = 0;
        return;
    }
    if  (c_max == r)
    {
        H = (g / 255.0f - b / 255.0f) / delta;
    }
    else if (c_max == g)
    {
        H = (b / 255.0f - r / 255.0f) / delta + 2.0f;
    }
    else if (c_max == b)
    {
        H = (r / 255.0f - g / 255.0f) / delta + 4.0f;
    }
    H *= 60.0f;
    H = H >= 0.0f ? H : H + 360.0f;
    H = modulo(H, 360.0f);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae
void CMaaHSVRGBcolor::HSV2RGB(float H, float S, float V, float& r, float& g, float& b)
{
    float C = V * S;
    float H_ = H / 60.0f;
    float X = modulo(H_, 2.0f) - 1.0f;
    X = X >= 0 ? X : -X;
    X = C * (1 - X);
    RGBColor rgb1{};
    if  (0.0f <= H_ && H_ <= 1.0f)
    {
        rgb1 = { C, X, 0 };
    }
    else if (1.0f < H_ && H_ <= 2.0f)
    {
        rgb1 = { X, C, 0 };
    }
    else if (2.0f < H_ && H_ <= 3.0f)
    {
        rgb1 = { 0, C, X };
    }
    else if (3.0f < H_ && H_ <= 4.0f)
    {
        rgb1 = { 0, X, C };
    }
    else if (4.0f < H_ && H_ <= 5.0f)
    {
        rgb1 = { X, 0, C };
    }
    else if (5.0f < H_ && H_ <= 6.0f)
    {
        rgb1 = { C, 0, X };
    }
    float m = V - C;
    r = rgb1.R + m;
    g = rgb1.G + m;
    b = rgb1.B + m;
}
void CMaaHSVRGBcolor::F2B_RGB(float r, float g, float b, _byte& R, _byte& G, _byte& B)
{
    int y = static_cast<int>(256 * r);
    R = y <= 255 ? y : 255;
    y = static_cast<int>(256 * g);
    G = y <= 255 ? y : 255;
    y = static_cast<int>(256 * b);
    B = y <= 255 ? y : 255;
}
void CMaaHSVRGBcolor::HSV2RGB(float H, float S, float V, _byte& r, _byte& g, _byte& b)
{
    float _r, _g, _b;
    HSV2RGB(H, S, V, _r, _g, _b);
    F2B_RGB(_r, _g, _b, r, g, b);
}

int CMaaHSVRGBcolor::test_CMaaHSVRGBcolor()
{
    /*
    float x = modulo(101.f, 10.f);
    printf("%f\n", x);

    x = modulo(102.f, 10.f);
    printf("%f\n", x);

    double y = modulo(102., 10.);
    printf("%f\n", y);
    */

    _byte r, g, b;
    float H, S, V;

    RGB2HSV(0, 0, 0, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("black  = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    RGB2HSV(255, 255, 255, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("white  = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    RGB2HSV(255, 0, 0, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("red    = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    RGB2HSV(0, 255, 0, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("green  = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    RGB2HSV(0, 0, 255, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("blue   = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    RGB2HSV(255, 255, 0, H, S, V);
    HSV2RGB(H, S, V, r, g, b);
    printf("yellow = HSV(%f, %f, %f)\t= RGB(%d, %d, %d)\n", H, S, V, r, g, b);

    return 0;
}
