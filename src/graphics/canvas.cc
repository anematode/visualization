//
// Created by Timothy Herchen on 1/26/18.
//

#include "canvas.h"

namespace Sine::Graphics {
    Canvas::Canvas(int width, int height) : Pixmap<RGBA>(width, height) {

    }

    Canvas::Canvas(const std::string &filename) : Pixmap<RGBA>(
            ImageLoader<RGBAMap>::loadAny(filename)) { // Load file using ImageLoader

    }

    Canvas::Canvas(const Canvas &p) : Pixmap<RGBA>(p.getWidth(), p.getHeight()) {
        width = p.getWidth();
        height = p.getHeight();

        area = p.area;

        copyFrom(p); // Copy pixel data from p
    }

    Canvas::Canvas(const Pixmap<RGBA> &p) : Pixmap<RGBA>(p.getWidth(), p.getHeight()) {
        copyFrom(p); // Copy pixel data from p
    }

    // Use ImageConverter to convert between given Pixmaps to RGBAMap
    typedef ImageConverter<RGBAMap> RGBAMapConverter;

    Canvas::Canvas(const Pixmap<RGB> &p) : Pixmap<RGBA>(p.getWidth(), p.getHeight()) {
        copyFrom(RGBAMapConverter::convert(p));
    }

    Canvas::Canvas(const Graymap &p) : Pixmap<RGBA>(p.getWidth(), p.getHeight()) {
        copyFrom(RGBAMapConverter::convert(p));
    }

    Canvas::Canvas(const Bitmap &p) : Pixmap<RGBA>(p.getWidth(), p.getHeight()) {
        copyFrom(RGBAMapConverter::convert(p));
    }

    void Canvas::fill(Color color) {
        std::fill_n(pixels, area, color.rgba());
    }

    void Canvas::clear() {
        fill(RGBA(255, 255, 255, 0));
    }
}
