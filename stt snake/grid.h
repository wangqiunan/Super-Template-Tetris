#pragma once

#include "list.h"
#include "functor.h"
#include "printer.h"
#include "position.h"
#include "utility.h"

/**
    Two dimensional grid of values.
    
    Stores a list of column lists that store cells.
*/
template <typename r>
struct Grid {
    using rows = r;
    
    static const size_t width = get_t<0, r>::size;
    static const size_t height = rows::size;
    
    template <typename pos>
    using nextPosition = Position<
        (pos::x + 1) % width,
        pos::x + 1 == width ? pos::y + 1 : pos::y>;
};

/**
    Generate an width x height grid of `value`.
*/
template <size_t width, size_t height, typename value>
using gen_grid = Grid<gen_t<height, gen_t<width, value>>>;

/**
    Get the element at `pos(x, y)` in a grid.
*/
template <typename pos, typename grid>
using grid_get = get_t<pos::x, get_t<pos::y, typename grid::rows>>;

/**
    Set the element at `pos(x, y)` in a grid to `value`.
*/
template <typename pos, typename value, typename g>
using grid_put = Grid<
    put_t<
        pos::y,
        put_t<pos::x, value, get_t<pos::y, typename g::rows>>,
        typename g::rows>>;

/**
    Run a function against a cell in the grid.
*/
template <typename f, typename pos, typename grid>
using grid_check = call<f, grid_get<pos, grid>>;


/**
    Transform a grid into a grid of cordinate value pairs.
*/
template <typename g>
struct GridZipPositions {
    struct inner {
        template <typename p, typename c>
        struct apply {
            using pos = car<p>;
            using grid = caar<p>;
            using nextPos = typename grid::template nextPosition<pos>;
            using type = List<
                nextPos,
                grid_put<pos, List<pos, c>, grid>>;
        };
    };
    
    using type = caar<fold<
        inner,
        List<Position<0, 0>, g>,
        g>>;
};

template <typename g>
using grid_zip_positions = typename GridZipPositions<g>::type;

/*------------------------------------------------------------------------------
    Printer
*/
template <>
struct Printer<Grid<List<>>>
{
    static void Print(std::ostream& output) { /* noop */ }
};

template <typename x, typename... xs>
struct Printer<Grid<List<x, xs...>>>
{
    static void Print(std::ostream& output)
    {
        Printer<x>::Print(output);
        output << "\n";
        Printer<Grid<List<xs...>>>::Print(output);
    }
};

/*------------------------------------------------------------------------------
    Foldable
*/
template <typename f, typename z, typename rows>
struct Foldable<f, z, Grid<rows>> {
    struct inner {
        template <typename p, typename c>
        using apply = identity<fold<f, p, c>>;
    };

    using type = fold<
        inner,
        z,
        rows>;
};

/*------------------------------------------------------------------------------
    Functor
*/
template <typename f, typename rows>
struct Fmap<f, Grid<rows>> {
    struct inner {
        template <typename x>
        using apply = identity<fmap_t<f, x>>;
    };
    
    using type = Grid<
        fmap_t<
            inner,
            rows>>;
};

/*------------------------------------------------------------------------------
    Serialize
*/
template <typename rows>
struct Serialize<Grid<rows>>
{
    static std::ostream& Write(std::ostream& output)
    {
        output << "grid<";
        Serialize<rows>::Write(output);
        return output << ">";
    }
};