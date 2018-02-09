//
// Created by Timothy Herchen on 2/7/18.
//

#include <algorithm>
#include <iostream>
#include "line.h"

namespace Sine {
    namespace Graphics {
        namespace Algorithms {
            bool isVertical(int x1, int, int x2, int) {
                return x1 == x2;
            }

            bool isHorizontal(int, int y1, int, int y2) {
                return y1 == y2;
            }

            double lineSlope(int x1, int y1, int x2, int y2) {
                return (y2 - y1) / float(x2 - x1);
            }

            float lineYAt(int x1, int y1, int x2, int y2, int x_sample) {
                if (isVertical(x1, y1, x2, y2))
                    return std::numeric_limits<float>::quiet_NaN();

                return (x_sample - x1) * (y2 - y1) / float(x2 - x1) + y1;
            }

            float lineXAt(int x1, int y1, int x2, int y2, int y_sample) {
                if (isHorizontal(x1, y1, x2, y2))
                    return std::numeric_limits<float>::quiet_NaN();

                return (y_sample - y1) * (x2 - x1) / float(y2 - y1) + x1;
            }

            _line trimLine(int x1, int y1, int x2, int y2, int r_x1, int r_y1, int r_x2, int r_y2) {
                if (r_x1 > r_x2)
                    std::swap(r_x1, r_x2);

                if (r_y1 > r_y2)
                    std::swap(r_y2, r_y2);

                float topX = lineXAt(x1, y1, x2, y2, r_y1);
                float leftY = lineYAt(x1, y1, x2, y2, r_x1);
                float bottomX = lineXAt(x1, y1, x2, y2, r_y2);
                float rightY = lineYAt(x1, y1, x2, y2, r_x2);

                int n_x1 = x1, n_x2 = x2, n_y1 = y1, n_y2 = y2;

                int intersection_count = 0;

                int minX = std::min(x1, x2);
                int minY = std::min(y1, y2);

                int maxX = std::max(x1, x2);
                int maxY = std::max(y1, y2);

                if ((minY < r_y1) && (r_y1 < maxY) && !std::isnan(topX) && (minX < topX) && (topX < maxX)) {
                    // It intersected the top edge, so we adjust the original point with smaller y value

                    std::cout << __LINE__ << std::endl;
                    if (y2 < y1) {
                        n_y2 = r_y1;
                        n_x2 = (int) topX;
                    } else {
                        n_y1 = r_y1;
                        n_x1 = (int) topX;
                    }

                    intersection_count++;
                }

                if ((minY < r_y2) && (r_y2 < maxY) && !std::isnan(bottomX) && (minX < bottomX) && (bottomX < maxX)) {
                    // It intersected the bottom edge, so we adjust the original point with higher y value

                    std::cout << bottomX << std::endl;

                    if (y2 > y1) {
                        n_y2 = r_y2;
                        n_x2 = (int) bottomX;
                    } else {
                        n_y1 = r_y2;
                        n_x1 = (int) bottomX;
                    }

                    intersection_count++;
                }

                if ((minX < r_x1) && (r_x1 < maxX) && !std::isnan(leftY) && (minY < leftY) && (leftY < maxY)) {
                    // It intersected the left edge, so we adjust the original point with smaller x value

                    std::cout << __LINE__ << std::endl;

                    if (x2 < x1) {
                        n_y2 = (int) leftY;
                        n_x2 = r_x1;
                    } else {
                        n_y1 = (int) leftY;
                        n_x1 = r_x1;
                    }

                    intersection_count++;
                }

                if ((minX < r_x2) && (r_x2 < maxX) && !std::isnan(rightY) && (minY < rightY) && (rightY < maxY)) {
                    // It intersected the right edge, so we adjust the original point with larger x value

                    std::cout << __LINE__ << std::endl;

                    if (x2 > x1) {
                        n_y2 = (int) leftY;
                        n_x2 = r_x2;
                    } else {
                        n_y1 = (int) leftY;
                        n_x1 = r_x2;
                    }

                    intersection_count++;
                }

                return {n_x1, n_y1, n_x2, n_y2};
            }
        }
    }
}