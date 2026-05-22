
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/gd_helper.h

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

#ifndef NO_GD

// apt install libgd3 libgd-dev
#include <gd.h>

#include <gdfonts.h>
#include <gdfontl.h>
#include <gdfontt.h>
#include <gdfontmb.h>

#include <gdfonts.h>
#include <gdfontl.h>
#include <gdfontg.h>
#include <gdfontt.h>
#include <gdfontmb.h>

//gdcache.h
//gdfontg.h
//gdfx.h
//gd_io.h

class CMaaGD_io_iface : public gdIOCtx
{
    static int __getC(struct gdIOCtx * ctx);
    static int __getBuf(struct gdIOCtx * ctx, void *ptr, int len);
    static void __putC(struct gdIOCtx * ctx, int ch);
    static int __putBuf(struct gdIOCtx * ctx, const void * ptr, int len);
    static int __seek(struct gdIOCtx * ctx, const int pos);
    static long __tell(struct gdIOCtx * ctx);
    static void __gd_free(struct gdIOCtx * ctx);

public:
    CMaaGD_io_iface();
    virtual ~CMaaGD_io_iface();

    virtual int getC() = 0;
    virtual int getBuf(void *ptr, int len) = 0;
    virtual void putC(int ch) = 0;
    virtual int putBuf(const void * ptr, int len) = 0;
    // seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
    virtual int seek(const int pos) = 0;
    virtual long tell() = 0;
    virtual void gd_free() {};
};

class CMaaGD_io_MemoryCtx : public CMaaGD_io_iface
{
    char m_Mode; // 'r' / 'w'
    CMaaString m_srcmem;
    int m_limit, m_pos, m_size;

    CMaaPtrAE_<char, 1> m_memory;

public:
    CMaaGD_io_MemoryCtx(const CMaaString &srcmem) noexcept;
    CMaaGD_io_MemoryCtx(int limit = 100 * 1024 * 1024);
    ~CMaaGD_io_MemoryCtx();
    CMaaString GetData();
    int getC();
    int getBuf(void *ptr, int len);
    void putC(int ch);
    int putBuf(const void * ptr, int len);
    // seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
    int seek(const int pos);
    long tell();
    void gd_free();
    CMaaString GetStatistics();
};

//#define TTF_FONT_PATH "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif-Bold.ttf"
//#define TTF_FONT_PATH "/usr/share/fonts/truetype/Vera.ttf"
//#define TTF_FONT_PATH "/usr/share/fonts/bitstream-vera/Vera.ttf"
//#define TTF_FONT_PATH "/usr/share/fonts/truetype/Vera.ttf"
//#define TTF_FONT_PATH "/var/www/maa/data/html/maasoftware.ru/ref/Vera.ttf"
#define TTF_FONT_PATH1 "/var/www/maa/data/html/maasoftware.ru/ref/Vera.ttf"
#define TTF_FONT_PATH2 "/var/www/maa/data/html/maasoftware.ru/ref/times.ttf"
#define TTF_FONT_PATH TTF_FONT_PATH2

class gd_Image //Helper
{
    gdImagePtr im;

public:
    int BackgroundColor;
    int white;
    int black;
    int TextColor;
    //int TextColor2;
    int m_LastX, m_LastY, m_LastColor;

    gdFontPtr pTinyFont,
    pSmallFont, // normal
    pMediumBoldFont,
    pLargeFont,
    pGiantFont;

    enum eImageType
    {
        eError = -1,
        eNone = 0,
        ePng,
        eGif,
        eBmp,
        eJpeg
    };

    enum eColorIndex
    {
        estub = -100,
        eBackgroundColor,
        eWhiteColor,
        eBlackColor,
        eTextColor
    };
    int GetDefaultColor(int defaultcolor); //eTextColor and so on

    gd_Image();
    ~gd_Image();
    int GetColorExact(int rgb, int defaultcolor = eTextColor);
    int GetColor(int rgb, int defaultcolor = eTextColor);
    int GetColorExact(int r, int g, int b, int defaultcolor = eTextColor);
    int GetColor(int r, int g, int b, int defaultcolor = eTextColor);
    bool AcquireColorsBasic();
    bool Create(int sx, int sy, bool bTransparent = false);
    void FillRect(int x, int y, int sx, int sy, int Color);
    void SetTransparentColor(int Color);
    bool Destroy();
    bool IsValid();

    void Line(int x1, int y1, int x2, int y2, int color /*= -1*/);
    void MoveTo(int x, int y);
    void LineTo(int x2, int y2, int color = -1);
    void Arc(int centx, int centy, int w, int h, int start, int end, int color /*= -1*/);

    int GenerateImage(int Type, CMaaString &Data, CMaaString &ContentType, int JpegQuality = 70);
    bool Load(CMaaString FileName, bool bThrow = false, int Type = -1, bool bAutoDestroy = false);
    bool LoadFromMemory(CMaaString Data, CMaaString Ext, bool bThrow = false, int Type = -1, bool bAutoDestroy = false);
    bool Save(CMaaString FileName, CMaaString Mode = "WCD|SrSw", bool bThrow = false, int Type = -1, int JpegQuality = -1);
    CMaaString GetInMemory(CMaaString Ext, bool bThrow = false, int Type = -1, int JpegQuality = -1);
    int GetTypeByFileNameExtention(CMaaString FileName);
    int GetTypeByExtention(CMaaString ext);
    CMaaString GetExtentionByType(int Type, int dot = 1);

    //int brect[8];

    // 0 lower left corner,  X position
    // 1 lower left corner,  Y position
    // 2 lower right corner, X position
    // 3 lower right corner, Y position
    // 4 upper right corner, X position
    // 5 upper right corner, Y position
    // 6 upper left corner,  X position
    // 7 upper left corner,  Y position

#define D_bAutoAddTransparentColor true

    const char * GetTextSize(CMaaString Text, int * pSizeX, int * pSizeY, int * brect/*int[8]*/ = nullptr, CMaaString str_ttf_font_path = TTF_FONT_PATH, double ptsize = 32.0, double angle = 0.0);
    const char * DrawText(CMaaString Text, int x, int y, int Color = -1, int * brect/*int[8]*/ = nullptr, CMaaString str_ttf_font_path = TTF_FONT_PATH, double ptsize = 32.0, double angle = 0.0);
    const char * DrawText2(CMaaString Text, int x, int y, int Color = -1, double ptsize = 10.0, CMaaString str_ttf_font_path = TTF_FONT_PATH);
    const char * GetTextSize2(CMaaString Text, int * pSizeX, int * pSizeY, double ptsize = 10.0, CMaaString str_ttf_font_path = TTF_FONT_PATH);
    bool GetFixedFontTextSize(CMaaString Text, gdFontPtr pFont, int * pSizeX, int * pSizeY);
    bool DrawText(CMaaString Text, gdFontPtr pFont, int x, int y, int Color = -1);
    bool GetImageSize(int * pSizeX = nullptr, int * pSizeY = nullptr, bool bResetToZeroOnError = true);
    bool GetSize(int * pSizeX = nullptr, int * pSizeY = nullptr, bool bResetToZeroOnError = true);
    bool CopyResized(gd_Image &SrcImage, int srcX, int srcY, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH, bool bAutoAddTransparentColor = D_bAutoAddTransparentColor);
    bool CopyResized(gd_Image &SrcImage, bool bAutoAddTransparentColor = D_bAutoAddTransparentColor);
    bool Copy(gd_Image &SrcImage, int srcX, int srcY, int W, int H, int dstX, int dstY, bool bAutoAddTransparentColor = D_bAutoAddTransparentColor);
    int GetPixel(int x, int y);
    gdImagePtr GetImPtr()
    {
        return im;
    }
    bool Rotate(int angle, bool bAutoAddTransparentColor = D_bAutoAddTransparentColor/**/); // +/- 90,180
    void Swap(gd_Image &That);
};
#endif // #ifndef NO_GD

// Hue,Saturation,Value <--> Red,Green,Blue conversions
namespace CMaaHSVRGBcolor
{
    struct HSVColor
    {
        float H, S, V;
    };
    struct RGBColor
    {
        float R, G, B;
    };

    constexpr float modulo(float dividend, float divisor)
    {
        //const auto quotient = dividend / divisor;
        //return divisor * (quotient - static_cast<uint8_t>(quotient));
        return dividend - divisor * static_cast<uint8_t>(dividend / divisor /*quotient*/);
    }
    constexpr _byte max2(_byte a, _byte b) noexcept
    {
        return a > b ? a : b;
    }
    constexpr _byte min2(_byte a, _byte b) noexcept
    {
        return a < b ? a : b;
    }
    constexpr _byte max3(_byte a, _byte b, _byte c) noexcept
    {
        return max2(max2(a, b), c);
    }
    constexpr _byte min3(_byte a, _byte b, _byte c) noexcept
    {
        return min2(min2(a, b), c);
    }

    void RGB2HSV(_byte r, _byte g, _byte b, float& H, float& S, float& V);
    void HSV2RGB(float H, float S, float V, float& r, float& g, float& b);
    void F2B_RGB(float r, float g, float b, _byte& R, _byte& G, _byte& B);
    void HSV2RGB(float H, float S, float V, _byte& r, _byte& g, _byte& b);
    int test_CMaaHSVRGBcolor();
}
