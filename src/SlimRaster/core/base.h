#pragma once

#include <cmath>

#if defined(__clang__)
    #define COMPILER_CLANG 1
    #define COMPILER_CLANG_OR_GCC 1
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC 1
    #define COMPILER_CLANG_OR_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#endif

#ifdef __CUDACC__
    #ifndef NDEBUG
        #include <stdio.h>
        #include <stdlib.h>
        inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true) {
            if (code != cudaSuccess) {
                fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code) , file, line);
                if (abort) exit(code);
            }
        }
        #define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
        #ifndef XPU
            #define XPU __device__ __host__
        #endif
        #ifndef INLINE_XPU
            #define INLINE_XPU __device__ __host__
        #endif
        #ifndef INLINE
            #define INLINE
        #endif
    #else
        #ifndef XPU
            #define XPU __device__ __host__
        #endif
        #ifndef INLINE_XPU
            #define INLINE_XPU __device__ __host__ __forceinline__
        #endif
        #ifndef INLINE
            #define INLINE __forceinline__
        #endif
        #define gpuErrchk(ans) (ans);
    #endif

    #define checkErrors() gpuErrchk(cudaPeekAtLastError())
    #define uploadNto(cpu_ptr, gpu_ptr, N, offset) gpuErrchk(cudaMemcpy(&((gpu_ptr)[(offset)]), (cpu_ptr), sizeof((cpu_ptr)[0]) * (N), cudaMemcpyHostToDevice))
    #define uploadN(  cpu_ptr, gpu_ptr, N        ) gpuErrchk(cudaMemcpy(&((gpu_ptr)[0])       , (cpu_ptr), sizeof((cpu_ptr)[0]) * (N), cudaMemcpyHostToDevice))
    #define downloadN(gpu_ptr, cpu_ptr, N)         gpuErrchk(cudaMemcpy((cpu_ptr), &((gpu_ptr)[0])       , sizeof((cpu_ptr)[0]) * (N), cudaMemcpyDeviceToHost))
    #define downloadNto(gpu_ptr,cpu_ptr,N, offset) gpuErrchk(cudaMemcpy((cpu_ptr), &((gpu_ptr)[(offset)]), sizeof((cpu_ptr)[0]) * (N), cudaMemcpyDeviceToHost))
#else
    #ifndef XPU
        #define XPU
    #endif
    #ifndef INLINE
        #if (defined(SLIMMER) || !defined(NDEBUG))
            #define INLINE
        #elif defined(COMPILER_MSVC)
            #define INLINE inline __forceinline
        #elif defined(COMPILER_CLANG_OR_GCC)
            #define INLINE inline __attribute__((always_inline))
        #else
            #define INLINE inline
        #endif
    #endif
    #ifndef INLINE_XPU
        #define INLINE_XPU INLINE
    #endif
#endif

#if defined(COMPILER_CLANG)
    #define likely(x)   __builtin_expect(x, true)
    #define unlikely(x) __builtin_expect_with_probability(x, false, 0.95)
#else
    #define likely(x)   x
    #define unlikely(x) x
#endif

#ifdef COMPILER_CLANG
    #define ENABLE_FP_CONTRACT \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
        _Pragma("STDC FP_CONTRACT ON") \
        _Pragma("clang diagnostic pop")
#else
    #define ENABLE_FP_CONTRACT
#endif

#ifdef FP_FAST_FMAF
    #define fast_mul_add(a, b, c) fmaf(a, b, c)
#else
    ENABLE_FP_CONTRACT
    #define fast_mul_add(a, b, c) ((a) * (b) + (c))
#endif

#ifndef signbit
    #define signbit std::signbit
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long int  u32;
typedef unsigned long long u64;
typedef signed   short     i16;
typedef signed   long int  i32;

typedef float  f32;
typedef double f64;

#ifndef CANVAS_COUNT
#define CANVAS_COUNT 2
#endif

#define FONT_WIDTH 9
#define FONT_HEIGHT 12

#define TAU 6.28f
#define SQRT2_OVER_2 0.70710678118f
#define SQRT3 1.73205080757f
#define COLOR_COMPONENT_TO_FLOAT 0.00392156862f
#define FLOAT_TO_COLOR_COMPONENT 255.0f
#define DEG_TO_RAD 0.0174533f

#define MAX_COLOR_VALUE 0xFF
#define THREE_QUARTERS_COLOR_VALUE 0xC0
#define HALF_COLOR_VALUE 0x80
#define QUARTER_COLOR_VALUE 0x40

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define MEMORY_SIZE Gigabytes(1)
#define MEMORY_BASE Terabytes(2)

#define MAX_WIDTH 3840
#define MAX_HEIGHT 2160
#define MAX_WINDOW_SIZE (MAX_WIDTH * MAX_HEIGHT)

#define DEFAULT_WIDTH 480
#define DEFAULT_HEIGHT 360

#define WINDOW_CONTENT_PIXEL_SIZE 4
#define WINDOW_CONTENT_SIZE (MAX_WINDOW_SIZE * WINDOW_CONTENT_PIXEL_SIZE)

#define BOX__ALL_SIDES (BoxSide_Top | BoxSide_Bottom | BoxSide_Left | BoxSide_Right | BoxSide_Front | BoxSide_Back)
#define BOX__VERTEX_COUNT 8
#define BOX__EDGE_COUNT 12
#define GRID__MAX_SEGMENTS 101

#define CURVE_STEPS 360

#define CUBE_UV_COUNT 4
#define CUBE_NORMAL_COUNT 6
#define CUBE_VERTEX_COUNT 8
#define CUBE_TRIANGLE_COUNT 12

#define CAMERA_DEFAULT__FOCAL_LENGTH 2.0f
#define CAMERA_DEFAULT__TARGET_DISTANCE 10

#define NAVIGATION_DEFAULT__MAX_VELOCITY 5
#define NAVIGATION_DEFAULT__ACCELERATION 10
#define NAVIGATION_SPEED_DEFAULT__TURN   1
#define NAVIGATION_SPEED_DEFAULT__ORIENT 0.002f
#define NAVIGATION_SPEED_DEFAULT__ORBIT  0.002f
#define NAVIGATION_SPEED_DEFAULT__ZOOM   0.003f
#define NAVIGATION_SPEED_DEFAULT__DOLLY  1
#define NAVIGATION_SPEED_DEFAULT__PAN    0.03f

#define VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE 0.001f
#define VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE 1000.0f

#define fractionOf(x) ((x) - floorf(x))
#define oneMinusFractionOf(x) (1 - fractionOf(x))

INLINE_XPU f32 clampedValue(f32 value, f32 from, f32 to) {
    f32 mn = value < to ? value : to;
    return mn > from ? mn : from;
}

INLINE_XPU i32 clampedValue(i32 value, i32 from, i32 to) {
    i32 mn = value < to ? value : to;
    return mn > from ? mn : from;
}

INLINE_XPU f32 clampedValue(f32 value, f32 to) {
    return value < to ? value : to;
}

INLINE_XPU i32 clampedValue(i32 value, i32 to) {
    return value < to ? value : to;
}

INLINE_XPU f32 clampedValue(f32 value) {
    f32 mn = value < 1.0f ? value : 1.0f;
    return mn > 0.0f ? mn : 0.0f;
}

INLINE_XPU i32 clampedValue(i32 value) {
    i32 mn = value < 1 ? value : 1;
    return mn > 0 ? mn : 0;
}

INLINE_XPU f32 smoothStep(f32 from, f32 to, f32 t) {
    t = (t - from) / (to - from);
    return t * t * (3.0f - 2.0f * t);
}

INLINE_XPU f32 approach(f32 src, f32 trg, f32 diff) {
    f32 out;

    out = src + diff; if (trg > out) return out;
    out = src - diff; if (trg < out) return out;

    return trg;
}

template <typename T>
INLINE void swap(T *a, T *b) {
    T t = *a;
    *a = *b;
    *b = t;
}

template <typename T>
struct RangeOf {
    T first, last;

    RangeOf() : RangeOf{0, 0} {}
    RangeOf(T first, T last) : first{first}, last{last} {}
    RangeOf(const RangeOf<T> &other) : RangeOf{other.first, other.last} {}

    INLINE bool contains(i32 v) const { return (first <= v) && (v <= last); }
    INLINE bool bounds(i32 v) const { return (first < v) && (v < last); }
    INLINE bool operator!() const { return last < first; }
    INLINE bool operator[](T v) const { return contains(v); }
    INLINE bool operator()(T v) const { return bounds(v); }
    INLINE void operator+=(T offset) {first += offset; last += offset;}
    INLINE void operator-=(T offset) {first -= offset; last -= offset;}
    INLINE void operator*=(T factor) {first *= factor; last *= factor;}
    INLINE void operator/=(T factor) {factor = 1 / factor; first *= factor; last *= factor;}
    INLINE void operator-=(const RangeOf<T> &rhs) { sub(rhs.first, rhs.last); }
    INLINE RangeOf<T> operator-(const RangeOf<T> &rhs) const {
        RangeOf<T> result{first, last};
        result.sub(rhs.first, rhs.last);
        return result;
    }
    INLINE void sub(T sub_first, T sub_last) {
        if (sub_last < sub_first) {
            T tmp = sub_last;
            sub_last = sub_first;
            sub_first = tmp;
        }
        if (last < sub_first || sub_last < first) {
            first = -1;
            last = -2;
        } else {
            first = first < sub_first ? sub_first : first;
            last = sub_last < last ? sub_last : last;
        }
    }
};
typedef RangeOf<f32> Range;
typedef RangeOf<i32> RangeI;

template <typename T>
struct RectOf {
    union {
        struct {
            T left;
            T right;
            T top;
            T bottom;
        };
        struct {
            RangeOf<T> x_range, y_range;
        };
    };

    RectOf(const RectOf<T> &other) : RectOf{other.x_range, other.y_range} {}
    RectOf(const RangeOf<T> &x_range, const RangeOf<T> &y_range) : x_range{x_range}, y_range{y_range} {}
    RectOf(T left = 0, T right = 0, T top = 0, T bottom = 0) : left{left}, right{right}, top{top}, bottom{bottom} {}

    INLINE bool contains(T x, T y) const { return x_range.contains(x) && y_range.contains(y); }
    INLINE bool bounds(T x, T y) const { return x_range.bounds(x) && y_range.bounds(y); }
    INLINE bool operator!() const { return !x_range || !y_range; }
    INLINE bool isOutsideOf(const RectOf<T> &other) {
        return (
                other.right < left || right < other.left ||
                other.bottom < top || bottom < other.top
        );
    }
    INLINE void operator+=(T offset) {x_range += offset; y_range += offset;}
    INLINE void operator-=(T offset) {x_range -= offset; y_range -= offset;}
    INLINE void operator*=(T factor) {x_range *= factor; y_range *= factor;}
    INLINE void operator/=(T factor) {x_range /= factor; y_range /= factor;}
    INLINE void operator-=(const RectOf<T> &rhs) { sub(rhs.x_range, rhs.y_range); }
    INLINE RectOf<T> operator-(const RectOf<T> &rhs) const {
        RectOf<T> result{x_range, y_range};
        result.sub(rhs.x_range, rhs.y_range);
        return result;
    }
    INLINE void sub(const RangeOf<T> &other_x_range, const RangeOf<T> &other_y_range) {
        x_range -= other_x_range;
        y_range -= other_y_range;
    }
};
typedef RectOf<f32> Rect;
typedef RectOf<i32> RectI;

struct Turn {
    bool right{false};
    bool left{false};
};

struct Move {
    bool right{false};
    bool left{false};
    bool up{false};
    bool down{false};
    bool forward{false};
    bool backward{false};
};

enum CurveType {
    CurveType_None = 0,

    CurveType_Helix,
    CurveType_Coil,
    CurveType_Sphere,

    CurveType_Count
};
struct Curve {
    CurveType type{CurveType_None};
    f32 revolution_count{1}, thickness{0.1f};
};

enum GeometryType {
    GeometryType_None = 0,

    GeometryType_Mesh,
    GeometryType_Grid,
    GeometryType_Box,
    GeometryType_Curve,

    GeometryType_Count
};


enum Axis {
    Axis_X = 1,
    Axis_Y = 2,
    Axis_Z = 4
};

enum BoxSide {
    BoxSide_None = 0,
    BoxSide_Top  = 1,
    BoxSide_Bottom = 2,
    BoxSide_Left   = 4,
    BoxSide_Right  = 8,
    BoxSide_Front  = 16,
    BoxSide_Back   = 32
};

template <class T>
struct Orientation {
    T rotation{};

    INLINE_XPU Orientation() : rotation{T::Identity} {}
    INLINE_XPU explicit Orientation(f32 x_radians, f32 y_radians = 0, f32 z_radians = 0) {
        setRotation(x_radians, y_radians, z_radians);
    }

    INLINE_XPU void rotate(f32 x_radians, f32 y_radians, f32 z_radians) {
        setRotation(x + x_radians, y + y_radians, x + z_radians);
    }

    INLINE_XPU void rotate(f32 x_radians, f32 y_radians) {
        setRotation(x + x_radians, y + y_radians);
    }

    INLINE_XPU void setRotation(f32 x_radians, f32 y_radians, f32 z_radians) {
        x = x_radians;
        y = y_radians;
        z = z_radians;
        _update();
    }

    INLINE_XPU void setRotation(f32 x_radians, f32 y_radians) {
        x = x_radians;
        y = y_radians;
        _update();
    }

    INLINE_XPU void rotateAroundX(f32 radians) {
        setRotationAroundX(x + radians);
    }

    INLINE_XPU void rotateAroundY(f32 radians) {
        setRotationAroundY(y + radians);
    }

    INLINE_XPU void rotateAroundZ(f32 radians) {
        setRotationAroundZ(z + radians);
    }

    INLINE_XPU void setRotationAroundX(f32 radians) {
        x = radians;
        _update();
    }

    INLINE_XPU void setRotationAroundY(f32 radians) {
        y = radians;
        _update();
    }

    INLINE_XPU void setRotationAroundZ(f32 radians) {
        z = radians;
        _update();
    }

protected:
    f32 x, y, z;

    INLINE_XPU void _update() {
        rotation = T{};
        if (z != 0.0f) rotation = T::RotationAroundZ(z);
        if (x != 0.0f) rotation *= T::RotationAroundX(x);
        if (y != 0.0f) rotation *= T::RotationAroundY(y);
    }
};

enum ColorID {
    Black,
    White,
    Grey,

    Red,
    Green,
    Blue,

    Cyan,
    Magenta,
    Yellow,

    DarkRed,
    DarkGreen,
    DarkBlue,
    DarkGrey,

    BrightRed,
    BrightGreen,
    BrightBlue,
    BrightGrey,

    BrightCyan,
    BrightMagenta,
    BrightYellow,

    DarkCyan,
    DarkMagenta,
    DarkYellow
};

struct Color {
    union {
        struct { f32 red, green, blue; };
        struct { f32 r  , g    , b   ; };
    };

    INLINE_XPU Color(f32 value) : red{value}, green{value}, blue{value} {}
    INLINE_XPU Color(f32 red = 0.0f, f32 green = 0.0f, f32 blue = 0.0f) : red{red}, green{green}, blue{blue} {}
    INLINE_XPU Color(enum ColorID color_id) : Color{} {
        switch (color_id) {
            case Black: break;
            case White:
                red = 1.0f;
                green = 1.0f;
                blue = 1.0f;
                break;
            case Grey:
                red = 0.5f;
                green = 0.5f;
                blue = 0.5f;
                break;
            case DarkGrey:
                red = 0.25f;
                green = 0.25f;
                blue = 0.25f;
                break;
            case BrightGrey:
                red = 0.75f;
                green = 0.75f;
                blue = 0.75f;
                break;

            case Red:
                red = 1.0f;
                break;
            case Green:
                green = 1.0f;
                break;
            case Blue:
                blue = 1.0f;
                break;

            case DarkRed:
                red = 0.5f;
                break;
            case DarkGreen:
                green = 0.5f;
                break;
            case DarkBlue:
                blue = 0.5f;
                break;

            case DarkCyan:
                green = 0.5f;
                blue = 0.5f;
                break;
            case DarkMagenta:
                red = 0.5f;
                blue = 0.5f;
                break;
            case DarkYellow:
                red = 0.5f;
                green = 0.5f;
                break;

            case BrightRed:
                red = 1.0f;
                green = 0.5f;
                blue = 0.5f;
                break;
            case BrightGreen:
                red = 0.5f;
                green = 1.0f;
                blue = 0.5f;
                break;
            case BrightBlue:
                red = 0.5f;
                green = 0.5f;
                blue = 1.0f;
                break;

            case Cyan:
                blue = 1.0f;
                green = 1.0f;
                break;
            case Magenta:
                red = 1.0f;
                blue = 1.0f;
                break;
            case Yellow:
                red = 1.0f;
                green = 1.0f;
                break;

            case BrightCyan:
                green = 0.75f;
                blue = 0.75f;
                break;
            case BrightMagenta:
                red = 0.75f;
                blue = 0.75f;
                break;
            case BrightYellow:
                red = 0.75f;
                green = 0.75f;
                break;
        }
    }

    INLINE_XPU Color clamped() const {
        return {
            clampedValue(r),
            clampedValue(g),
            clampedValue(b)
        };
    }

    INLINE_XPU  void setByHex(i32 hex) {
        r = (float)((0xFF0000 & hex) >> 16) * COLOR_COMPONENT_TO_FLOAT;
        g = (float)((0x00FF00 & hex) >>  8) * COLOR_COMPONENT_TO_FLOAT;
        b = (float)( 0x0000FF & hex)        * COLOR_COMPONENT_TO_FLOAT;
    }

    INLINE_XPU Color& operator = (f32 value) {
        r = g = b = value;
        return *this;
    }

    INLINE_XPU Color& operator = (ColorID color_id) {
        *this  = Color(color_id);
        return *this;
    }

    INLINE_XPU Color operator + (const Color &rhs) const {
        return {
                r + rhs.r,
                g + rhs.g,
                b + rhs.b
        };
    }

    INLINE_XPU Color operator + (f32 scalar) const {
        return {
                r + scalar,
                g + scalar,
                b + scalar
        };
    }

    INLINE_XPU Color& operator += (const Color &rhs) {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        return *this;
    }

    INLINE_XPU Color& operator += (f32 scalar) {
        r += scalar;
        g += scalar;
        b += scalar;
        return *this;
    }

    INLINE_XPU Color operator - (const Color &rhs) const {
        return {
                r - rhs.r,
                g - rhs.g,
                b - rhs.b
        };
    }

    INLINE_XPU Color operator - (f32 scalar) const {
        return {
                r - scalar,
                g - scalar,
                b - scalar
        };
    }

    INLINE_XPU Color& operator -= (const Color &rhs) {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        return *this;
    }

    INLINE_XPU Color& operator -= (f32 scalar) {
        r -= scalar;
        g -= scalar;
        b -= scalar;
        return *this;
    }

    INLINE_XPU Color operator * (const Color &rhs) const {
        return {
            r * rhs.r,
            g * rhs.g,
            b * rhs.b
        };
    }

    INLINE_XPU Color operator * (f32 scalar) const {
        return {
            r * scalar,
            g * scalar,
            b * scalar
        };
    }

    INLINE_XPU Color& operator *= (const Color &rhs) {
        r *= rhs.r;
        g *= rhs.g;
        b *= rhs.b;
        return *this;
    }

    INLINE_XPU Color& operator *= (f32 scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    INLINE_XPU Color operator / (const Color &rhs) const {
        return {
                r / rhs.r,
                g / rhs.g,
                b / rhs.b
        };
    }

    INLINE_XPU Color operator / (f32 scalar) const {
        scalar = 1.0f / scalar;
        return {
                r * scalar,
                g * scalar,
                b * scalar
        };
    }

    INLINE_XPU Color& operator /= (const Color &rhs) {
        r /= rhs.r;
        g /= rhs.g;
        b /= rhs.b;
        return *this;
    }

    INLINE_XPU Color& operator /= (f32 scalar) {
        scalar = 1.0f / scalar;
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    INLINE_XPU Color lerpTo(const Color &to, f32 by) const {
        return (to - *this).scaleAdd(by, *this);
    }

    INLINE_XPU Color scaleAdd(f32 factor, const Color &to_be_added) const {
        return {
                fast_mul_add(r, factor, to_be_added.r),
                fast_mul_add(g, factor, to_be_added.g),
                fast_mul_add(b, factor, to_be_added.b)
        };
    }
};

struct Pixel {
    Color color;
    f32 opacity;

    INLINE_XPU Pixel(Color color, f32 opacity = 1.0f) : color{color}, opacity{opacity} {}
    INLINE_XPU Pixel(f32 red = 0.0f, f32 green = 0.0f, f32 blue = 0.0f, f32 opacity = 0.0f) : color{red, green, blue}, opacity{opacity} {}
    INLINE_XPU Pixel(enum ColorID color_id, f32 opacity = 1.0f) : Pixel{Color(color_id), opacity} {}

    INLINE_XPU Pixel operator * (f32 factor) const {
        return {
            color * factor,
            opacity * factor
        };
    }

    INLINE_XPU Pixel operator + (const Pixel &rhs) const {
        return {
            color + rhs.color,
            opacity + rhs.opacity
        };
    }

    INLINE_XPU Pixel& operator += (const Pixel &rhs) {
        color += rhs.color;
        opacity += rhs.opacity;
        return *this;
    }

    INLINE_XPU Pixel& operator *= (const Pixel &rhs) {
        color *= rhs.color;
        opacity *= rhs.opacity;
        return *this;
    }

    INLINE_XPU Pixel alphaBlendOver(const Pixel &background) const {
        return *this + background * (1.0f - opacity);
    }

    INLINE_XPU u32 asContent() const {
        u8 R = (u8)(color.r > 1.0f ? MAX_COLOR_VALUE : (FLOAT_TO_COLOR_COMPONENT * sqrt(color.r)));
        u8 G = (u8)(color.g > 1.0f ? MAX_COLOR_VALUE : (FLOAT_TO_COLOR_COMPONENT * sqrt(color.g)));
        u8 B = (u8)(color.b > 1.0f ? MAX_COLOR_VALUE : (FLOAT_TO_COLOR_COMPONENT * sqrt(color.b)));
        return R << 16 | G << 8 | B;
    }
};

struct Image {
    u16 width, height;
    Pixel *pixels;
    Pixel* operator[] (int row) const { return pixels + row*width; }
};

#define PIXEL_SIZE (sizeof(Pixel))
#define CANVAS_PIXELS_SIZE (MAX_WINDOW_SIZE * PIXEL_SIZE * 4)
#define CANVAS_DEPTHS_SIZE (MAX_WINDOW_SIZE * sizeof(f32) * 4)
#define CANVAS_SIZE (CANVAS_PIXELS_SIZE + CANVAS_DEPTHS_SIZE)

struct Dimensions {
    u32 width_times_height{(u32)DEFAULT_WIDTH * (u32)DEFAULT_HEIGHT};
    f32 width_over_height{(f32)DEFAULT_WIDTH / (f32)DEFAULT_HEIGHT};
    f32 height_over_width{(f32)DEFAULT_HEIGHT / (f32)DEFAULT_WIDTH};
    f32 f_width{(f32)DEFAULT_WIDTH};
    f32 f_height{(f32)DEFAULT_HEIGHT};
    f32 h_width{(f32)DEFAULT_WIDTH * 0.5f};
    f32 h_height{(f32)DEFAULT_HEIGHT * 0.5f};
    u16 width{DEFAULT_WIDTH};
    u16 height{DEFAULT_HEIGHT};
    u16 stride{DEFAULT_WIDTH};

    Dimensions() = default;
    Dimensions(u16 Width, u16 Height, u16 Stride) :
            width_times_height{(u32)Width * (u32)Height},
            width_over_height{(f32)Width / (f32)Height},
            height_over_width{(f32)Height / (f32)Width},
            f_width{(f32)Width},
            f_height{(f32)Height},
            h_width{(f32)Width * 0.5f},
            h_height{(f32)Height * 0.5f},
            width{Width},
            height{Height},
            stride{Stride}
    {}

    void update(u16 Width, u16 Height) {
        width = Width;
        height = Height;
        stride = Width;
        width_times_height = width * height;
        f_width  = (f32)width;
        f_height = (f32)height;
        h_width  = f_width  * 0.5f;
        h_height = f_height * 0.5f;
        width_over_height  = f_width  / f_height;
        height_over_width  = f_height / f_width;
    }
};

namespace controls {
    namespace key_map {
        u8 ctrl{0};
        u8 alt{0};
        u8 shift{0};
        u8 space{0};
        u8 tab{0};
        u8 escape{0};
        u8 left{0};
        u8 right{0};
        u8 up{0};
        u8 down{0};
    }

    namespace is_pressed {
        bool ctrl{false};
        bool alt{false};
        bool shift{false};
        bool space{false};
        bool tab{false};
        bool escape{false};
        bool left{false};
        bool right{false};
        bool up{false};
        bool down{false};
    }
}

namespace os {
    void* getMemory(u64 size, u64 base = 0);
    void setWindowTitle(char* str);
    void setWindowCapture(bool on);
    void setCursorVisibility(bool on);
    void closeFile(void *handle);
    void* openFileForReading(const char* file_path);
    void* openFileForWriting(const char* file_path);
    bool readFromFile(void *out, unsigned long, void *handle);
    bool writeToFile(void *out, unsigned long, void *handle);
}

namespace timers {
    u64 getTicks();
    u64 ticks_per_second;
    f64 seconds_per_tick;
    f64 milliseconds_per_tick;
    f64 microseconds_per_tick;
    f64 nanoseconds_per_tick;

    struct Timer {
        f32 delta_time{0};

        u64 ticks_before{0};
        u64 ticks_after{0};
        u64 ticks_diff{0};
        u64 ticks_of_last_report{0};

        u64 accumulated_ticks{0};
        u64 accumulated_frame_count{0};

        u64 seconds{0};
        u64 milliseconds{0};
        u64 microseconds{0};
        u64 nanoseconds{0};

        f64 average_frames_per_tick{0.0};
        f64 average_ticks_per_frame{0.0};
        u16 average_frames_per_second{0};
        u16 average_milliseconds_per_frame{0};
        u16 average_microseconds_per_frame{0};
        u16 average_nanoseconds_per_frame{0};

        Timer() noexcept : ticks_before{getTicks()}, ticks_after{getTicks()}, ticks_of_last_report{getTicks()} {};

        INLINE void accumulate() {
            ticks_diff = ticks_after - ticks_before;
            accumulated_ticks += ticks_diff;
            accumulated_frame_count++;

            seconds = (u64) (seconds_per_tick * (f64) (ticks_diff));
            milliseconds = (u64) (milliseconds_per_tick * (f64) (ticks_diff));
            microseconds = (u64) (microseconds_per_tick * (f64) (ticks_diff));
            nanoseconds = (u64) (nanoseconds_per_tick * (f64) (ticks_diff));
        }

        INLINE void average() {
            average_frames_per_tick = (f64) accumulated_frame_count / (f64) accumulated_ticks;
            average_ticks_per_frame = (f64) accumulated_ticks / (f64) accumulated_frame_count;
            average_frames_per_second = (u16) (average_frames_per_tick * (f64) ticks_per_second);
            average_milliseconds_per_frame = (u16) (average_ticks_per_frame * milliseconds_per_tick);
            average_microseconds_per_frame = (u16) (average_ticks_per_frame * microseconds_per_tick);
            average_nanoseconds_per_frame = (u16) (average_ticks_per_frame * nanoseconds_per_tick);
            accumulated_ticks = accumulated_frame_count = 0;
        }

        INLINE void beginFrame() {
            ticks_before = getTicks();
            ticks_diff = ticks_before - ticks_after;
            delta_time = (f32) ((f64) ticks_diff * seconds_per_tick);
        }

        INLINE void endFrame() {
            ticks_after = getTicks();
            accumulate();
            if (accumulated_ticks >= (ticks_per_second / 8))
                average();
        }
    };
}

namespace mouse {
    struct Button {
        i32 down_pos_x, down_pos_y, up_pos_x, up_pos_y, double_click_pos_x, double_click_pos_y;

        bool is_pressed{false}, double_clicked{false};

        void down(i32 x, i32 y) {
            is_pressed = true;

            down_pos_x = x;
            down_pos_y = y;
        }

        void up(i32 x, i32 y) {
            is_pressed = false;

            up_pos_x = x;
            up_pos_y = y;
        }

        void doubleClick(i32 x, i32 y) {
            double_clicked = true;
            double_click_pos_x = x;
            double_click_pos_y = y;
        }
    };

    Button middle_button, right_button, left_button;

    i32 pos_x, pos_y, pos_raw_diff_x, pos_raw_diff_y, movement_x, movement_y;
    f32 wheel_scroll_amount{0};

    bool moved{false};
    bool is_captured{false};
    bool double_clicked{false};
    bool wheel_scrolled{false};

    void resetChanges() {
        moved = false;
        double_clicked = false;
        wheel_scrolled = false;
        wheel_scroll_amount = 0;
        pos_raw_diff_x = 0;
        pos_raw_diff_y = 0;
        right_button.double_clicked = false;
        left_button.double_clicked = false;
        middle_button.double_clicked = false;
    }

    void scroll(f32 amount) {
        wheel_scroll_amount += amount * 100;
        wheel_scrolled = true;
    }

    void setPosition(i32 x, i32 y) {
        pos_x = x;
        pos_y = y;
    }

    void move(i32 x, i32 y) {
        movement_x = x - pos_x;
        movement_y = y - pos_y;
        moved = true;
    }

    void moveRaw(i32 x, i32 y) {
        pos_raw_diff_x += x;
        pos_raw_diff_y += y;
        moved = true;
    }
}

namespace memory {
    u8 *canvas_memory{nullptr};
    u64 canvas_memory_capacity = CANVAS_SIZE * CANVAS_COUNT;

    typedef void* (*AllocateMemory)(u64 size);

    struct MonotonicAllocator {
        u8* address{nullptr};
        u64 capacity{0};
        u64 occupied{0};

        MonotonicAllocator() = default;

        explicit MonotonicAllocator(u64 Capacity, u64 starting = 0) {
            capacity = Capacity;
            address = (u8*)os::getMemory(Capacity, starting);
        }

        void* allocate(u64 size) {
            if (!address) return nullptr;
            occupied += size;
            if (occupied > capacity) return nullptr;

            void* current_address = address;
            address += size;
            return current_address;
        }
    };
}

namespace window {
    u16 width{DEFAULT_WIDTH};
    u16 height{DEFAULT_HEIGHT};
    char* title{(char*)""};
    u32 *content{nullptr};
}