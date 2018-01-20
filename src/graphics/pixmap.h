#ifndef PIXMAP_DEFINED_
#define PIXMAP_DEFINED_

#include <string>

#include <algorithm>
#include <exception>
#include <iostream>

#include "../math/vec2.h"
#include "imageutils.h"
#include "color.h"
#include <fstream>

#include "stb_image_write.h"

#include <cmath>

namespace Vis {

    /**
  Pixmap is the base class for Graymap, Bitmap, RGBMap and RGBMap
  Basically it just abstracts a 2D array of pixels, given a template argument for the pixel itself
    **/

    template<typename PixelColor>
    class Pixmap {
    protected:
        PixelColor *pixels;
        int width;
        int height;
        size_t area;

    public:
        using PixelType = PixelColor;
        const static int ColorSize = sizeof(PixelColor);

        Pixmap(int width, int height);

        Pixmap(const Pixmap<PixelColor> &p);

        Pixmap(Pixmap &&p) noexcept;

        void copyFrom(const Pixmap &p);

        PixelColor *begin();

        PixelColor *end();

        ~Pixmap();

        inline void checkIndex(int index) const;

        inline void checkPair(int x, int y) const;

        inline int getWidth() const;

        inline int getHeight() const;

        PixelColor *getPixels() const;

        size_t getArea() const;

        void setPixelPointer(PixelColor *p);

        void setPixelPointer(void *p);

        inline bool indexContained(int index) const;

        inline bool pairContained(int x, int y) const;

        inline int pairToIndex(int x, int y) const;

        PixelColor getPixel(int index, bool safe = true) const;

        PixelColor getPixel(int x, int y, bool safe = true) const;

        PixelColor &getPixelRef(int index, bool safe = true);

        PixelColor &getPixelRef(int x, int y, bool safe = true);

        void setPixel(int index, PixelColor c, bool safe = true);

        void setPixel(int x, int y, PixelColor c, bool safe = true);

        bool exportToFile(std::string file,
                          ImageType type = ImageType::UNKNOWN);

        Pixmap sample(double x) const;

        void exportToBMP(std::string file);

        void exportToJPEG(std::string file, int quality = 90);

        void exportToGIF(std::string file);

        void exportToPNG(std::string file);

        void exportToPBM(std::string file);

        void exportToPPM(std::string file);

        void exportToPGM(std::string file);
    };

    template<typename PixelColor>
    Pixmap<PixelColor>::Pixmap(int w, int h) {
        pixels = new PixelColor[w * h];
        width = w;
        height = h;
        area = width * height;
    }

    template<typename PixelColor>
    Pixmap<PixelColor>::Pixmap(const Pixmap<PixelColor> &p) {
        pixels = new PixelColor[p.width * p.height];
        for (int i = 0; i < p.width * p.height; i++) {
            pixels[i] = p.pixels[i];
        }

        this->width = p.width;
        this->height = p.height;
        area = width * height;
    }

    template<typename PixelColor>
    PixelColor *Pixmap<PixelColor>::begin() {
        return pixels;
    }

    template<typename PixelColor>
    PixelColor *Pixmap<PixelColor>::end() {
        return pixels + area;
    }

    template<typename PixelColor>
    Pixmap<PixelColor>::Pixmap(Pixmap &&p) noexcept {
        if (p.width == width && p.height == height) {
            pixels = p.pixels;
            p.pixels = nullptr;
        } else {
            throw std::logic_error("Pixmaps must be of the same dimensions for a move constructor.");
        }
    }

    template<typename PixelColor>
    void Pixmap<PixelColor>::copyFrom(const Pixmap<PixelColor> &p) {
        if (p.getWidth() != width || p.getHeight() != height) {
            throw std::logic_error("Pixmaps must be of the same dimensions for copying.");
        } else {
            for (int i = 0; i < p.getWidth(); i++) {
                for (int j = 0; j < p.getHeight(); j++) {
                    setPixel(i, j, p.getPixel(i, j));
                }
            }
        }
    }

    template<typename PixelColor>
    Pixmap<PixelColor>::~Pixmap() {
        delete[] pixels;
    }

    template<typename PixelColor>
    int Pixmap<PixelColor>::getWidth() const {
        return width;
    }

    template<typename PixelColor> int Pixmap<PixelColor>::getHeight() const {
        return height;
    }

    template<typename PixelColor>
    size_t Pixmap<PixelColor>::getArea() const {
        return area;
    }

    template<typename PixelColor>
    void Pixmap<PixelColor>::setPixelPointer(PixelColor *p) {
        pixels = p;
    }

    template<typename PixelColor>
    void Pixmap<PixelColor>::setPixelPointer(void *p) {
        pixels = static_cast<PixelColor *>(p);
    }

    template<typename PixelColor>
    PixelColor *Pixmap<PixelColor>::getPixels() const {
        return pixels;
    }

    template<typename PixelColor>
    inline bool Pixmap<PixelColor>::indexContained(int index) const {
        return (0 <= index && index < static_cast<int>(area));
    }

    template<typename PixelColor>
    inline bool Pixmap<PixelColor>::pairContained(int x, int y) const {
        return (0 <= x && x < width && 0 <= y && y < width);
    }

    template<typename PixelColor>
    inline void Pixmap<PixelColor>::checkIndex(int index) const {
        if (!indexContained(index)) {
            throw std::out_of_range(
                    "Tried to access pixel at index = " + std::to_string(index) +
                    ", max index is " + std::to_string(area) + ".");
        }
    }

    template<typename PixelColor>
    inline void Pixmap<PixelColor>::checkPair(int x, int y) const {
        if (!pairContained(x, y)) {
            throw std::out_of_range(
                    "Tried to access pixel at x,y = " + Vec2f(x, y).toString() +
                    ", max dimensions are " + Vec2f(width, height).toString() + ".");
        }
    }

    template<typename PixelColor>
    inline int Pixmap<PixelColor>::pairToIndex(int x, int y) const {
        return y * width + x;
    }

    template<typename PixelColor>
    PixelColor Pixmap<PixelColor>::getPixel(int index, bool safe) const {
        if (safe)
            checkIndex(index);

        return pixels[index];
    }

    template<typename PixelColor>
    Pixmap<PixelColor> Pixmap<PixelColor>::sample(double b) const {

        int newWidth = width * b;
        int newHeight = height * b;

        Pixmap ret(newWidth, newHeight);

        if (newWidth == width && newHeight == height) {
            ret.copyFrom(*this);
            return ret;
        }

        for (int i = 0; i < newWidth; i++) {
            for (int j = 0; j < newHeight; j++) {
                ret.setPixel(i, j, getPixel(i / b, j / b, true));
            }
        }

        return ret;
    }

    template<typename PixelColor>
    PixelColor Pixmap<PixelColor>::getPixel(int x, int y, bool safe) const {
        if (safe)
            checkPair(x, y);

        return pixels[pairToIndex(x, y)];
    }

    template<typename PixelColor>
    PixelColor &Pixmap<PixelColor>::getPixelRef(int index, bool safe) {
        if (safe)
            checkIndex(index);

        return pixels[index];
    }

    template<typename PixelColor>
    PixelColor &Pixmap<PixelColor>::getPixelRef(int x, int y, bool safe) {
        if (safe)
            checkPair(x, y);

        return pixels[pairToIndex(x, y)];
    }

    template<typename PixelColor>
    void Pixmap<PixelColor>::setPixel(int index, PixelColor c, bool safe) {
        if (safe && !indexContained(index))
            return;

        pixels[index] = c;
    }

    template<typename PixelColor>
    void Pixmap<PixelColor>::setPixel(int x, int y, PixelColor c, bool safe) {
        if (safe && !pairContained(x, y))
            return;

        pixels[pairToIndex(x, y)] = c;
    }

    template<typename PixelColor>
    bool Pixmap<PixelColor>::exportToFile(std::string file, ImageType type) {
        guessType:

        switch (type) {
            case ImageType::UNKNOWN:
                type = extractImageType(file);

                if (type == ImageType::UNKNOWN) {
                    return false;
                }

                goto guessType;

            case ImageType::BMP:
                exportToBMP(file);
                break;

            case ImageType::JPEG:
                exportToJPEG(file);
                break;

            case ImageType::GIF:
                exportToGIF(file);
                break;

            case ImageType::PNG:
                exportToPNG(file);
                break;

            case ImageType::PBM:
                exportToPBM(file);
                break;

            case ImageType::PGM:
                exportToPGM(file);
                break;

            case ImageType::PPM:
                exportToPPM(file);
                break;
        }

        return true;
    }

    template<>
    inline void Pixmap<uint8_t>::exportToBMP(std::string path) {
        stbi_write_bmp(path.c_str(), getWidth(), getHeight(), 1,
                       static_cast<const void *>(getPixels()));
    }

    template<>
    inline void Pixmap<uint8_t>::exportToJPEG(std::string path, int quality) {
        stbi_write_jpg(path.c_str(), getWidth(), getHeight(), 1,
                       static_cast<const void *>(getPixels()), quality);
    }

    template<>
    inline void Pixmap<uint8_t>::exportToGIF(std::string file) {
        throw std::logic_error("GIF output is not implemented for Graymaps.");
    }

    template<>
    inline void Pixmap<uint8_t>::exportToPNG(std::string file) {
        stbi_write_png(file.c_str(), getWidth(), getHeight(), 1,
                       static_cast<const void *>(getPixels()), getWidth());
    }

    template<>
    inline void Pixmap<uint8_t>::exportToPBM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P4\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';

        uint8_t byteOut;
        int bitCount;

        int width = getWidth();

        for (int i = 0; i < getHeight(); i++) {
            bitCount = 0;
            byteOut = 0;

            for (int j = 0; j < width; j++) {
                if (bitCount > 7) {
                    file << byteOut;

                    bitCount = 0;
                    byteOut = 0;
                }

                if (getPixel(j, i, false) < 128) {
                    byteOut |= 1U << (7 - bitCount);
                }

                bitCount++;
            }

            if (bitCount != 0) {
                file << byteOut;
            }
        }

        file.close();
    }

    template<>
    inline void Pixmap<uint8_t>::exportToPGM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P5\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';
        file << "255";
        file << '\n';

        for (int index = 0; index < getArea(); index++) {
            file << getPixel(index, false);
        }

        file.close();
    }

    template<>
    inline void Pixmap<uint8_t>::exportToPPM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P6\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';
        file << "255";
        file << '\n';

        uint8_t c_out;

        for (int index = 0; index < getArea(); index++) {
            c_out = getPixel(index, false);
            file << c_out << c_out << c_out;
        }

        file.close();
    }

    template<>
    inline void Pixmap<bool>::exportToBMP(std::string path) {
        Pixmap<uint8_t> temp(getWidth(), getHeight());

        for (size_t index = 0; index < getArea(); index++) {
            temp.setPixel(index, (getPixel(index, false) ? 255 : 0));
        }

        temp.exportToBMP(path);
    }

    template<>
    inline void Pixmap<bool>::exportToJPEG(std::string path, int quality) {
        Pixmap<uint8_t> temp = Pixmap<uint8_t>(getWidth(), getHeight());

        for (size_t index = 0; index < getArea(); index++) {
            temp.setPixel(index, (getPixel(index, false) ? 255 : 0));
        }

        temp.exportToJPEG(path, quality);
    }

    template<>
    inline void Pixmap<bool>::exportToGIF(std::string path) {
        throw std::logic_error("GIF output is not implemented for Bitmaps.");
    }

    template<>
    inline void Pixmap<bool>::exportToPNG(std::string path) {
        Pixmap<uint8_t> temp = Pixmap<uint8_t>(getWidth(), getHeight());

        for (int index = 0; index < getArea(); index++) {
            temp.setPixel(index, (getPixel(index, false) ? 255 : 0));
        }

        temp.exportToPNG(path);
    }

    template<>
    inline void Pixmap<bool>::exportToPBM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P4\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';

        uint8_t byteOut;
        int bitCount;

        int width = getWidth();

        for (int i = 0; i < getHeight(); i++) {
            bitCount = 0;
            byteOut = 0;

            for (int j = 0; j < width; j++) {
                if (bitCount > 7) {
                    file << byteOut;

                    bitCount = 0;
                    byteOut = 0;
                }

                if (!getPixel(j, i, false)) {
                    byteOut |= 1U << (7 - bitCount);
                }

                bitCount++;
            }

            if (bitCount != 0) {
                file << byteOut;
            }
        }

        file.close();
    }

    template<>
    inline void Pixmap<bool>::exportToPGM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P5\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';
        file << "255";
        file << '\n';

        for (size_t index = 0; index < getArea(); index++) {
            file << static_cast<uint8_t>(getPixel(index, false) ? 255 : 0);
        }

        file.close();
    }

    template<>
    inline void Pixmap<bool>::exportToPPM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P6\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';
        file << "255";
        file << '\n';

        uint8_t c_out;

        for (size_t index = 0; index < getArea(); index++) {
            c_out = static_cast<uint8_t>(getPixel(index, false) ? 255 : 0);
            file << c_out << c_out << c_out;
        }

        file.close();
    }

    typedef Pixmap<bool> Bitmap;

    template<>
    inline void Pixmap<RGB>::exportToBMP(std::string path) {
        stbi_write_bmp(path.c_str(), getWidth(), getHeight(), 3,
                       static_cast<void *>(getPixels()));
    }

    template<>
    inline void Pixmap<RGB>::exportToJPEG(std::string path, int quality) {
        stbi_write_jpg(path.c_str(), getWidth(), getHeight(), 3,
                       static_cast<void *>(getPixels()), quality);
    }

    template<>
    inline void Pixmap<RGB>::exportToGIF(std::string file) {
        throw std::logic_error("GIF output is not implemented for RGBMaps.");
    }

    template<>
    inline void Pixmap<RGB>::exportToPNG(std::string file) {
        stbi_write_png(file.c_str(), getWidth(), getHeight(), 3,
                       static_cast<void *>(getPixels()), 3 * getWidth());
    }

    template<>
    inline void Pixmap<RGB>::exportToPBM(std::string path) {
        throw std::logic_error("PBM output is not implemented for RGBMaps.");
    }

    template<>
    inline void Pixmap<RGB>::exportToPGM(std::string path) {
        throw std::logic_error("PGM output is not implemented for RGBMaps.");
    }

    template<>
    inline void Pixmap<RGB>::exportToPPM(std::string path) {
        std::ofstream file;
        file.open(path, std::ios_base::out | std::ios_base::binary);

        file << "P6\n";
        file << std::to_string(getWidth()) << ' ' << std::to_string(getHeight())
             << '\n';
        file << "255";
        file << '\n';

        RGB c_out(0, 0, 0);

        for (int index = 0; index < getArea(); index++) {
            c_out = getPixel(index, false);
            file << c_out.r << c_out.g << c_out.b;
        }

        file.close();
    }

    typedef Pixmap<RGB> RGBMap;
    typedef Pixmap<uint8_t> Graymap;
    typedef Pixmap<RGBA> RGBAMap;

    template<>
    inline void Pixmap<RGBA>::exportToBMP(std::string path) {
        throw std::logic_error("BMP output is not implemented for RGBAMaps.");
    }

    template<>
    inline void Pixmap<RGBA>::exportToJPEG(std::string path, int quality) {
        stbi_write_jpg(path.c_str(), getWidth(), getHeight(), 4,
                       static_cast<void *>(getPixels()), quality);
    }

    template<>
    inline void Pixmap<RGBA>::exportToGIF(std::string file) {
        throw std::logic_error("GIF output is not implemented for RGBAMaps.");
    }

    template<>
    inline void Pixmap<RGBA>::exportToPNG(std::string file) {
        stbi_write_png(file.c_str(), getWidth(), getHeight(), 4,
                       static_cast<void *>(getPixels()), 4 * getWidth());
    }

    template<>
    inline void Pixmap<RGBA>::exportToPBM(std::string path) {
        throw std::logic_error("PBM output is not implemented for RGBAMaps.");
    }

    template<>
    inline void Pixmap<RGBA>::exportToPGM(std::string path) {
        throw std::logic_error("PGM output is not implemented for RGBAMaps.");
    }

    template<>
    inline void Pixmap<RGBA>::exportToPPM(std::string path) {
        throw std::logic_error("PPM output is not implemented for RGBAMaps.");
    }

} // namespace Vis


#endif
