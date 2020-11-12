#pragma once

#include <type_traits>
#include <cstdint>
#include <ratio>
#include <utility>

#if __cplusplus < 201703L
namespace std
{
    template< class T, class U >
    constexpr bool is_same_v = is_same<T, U>::value;

    template< class T >
    constexpr bool is_fundamental_v = is_fundamental<T>::value;
}
#endif

// TODO: convesrion to std::chrono?
// TODO: conversion ratio type
// TODO: conversion ratio value ???
// TODO: map si_descriptor_v to coversion ratio type
// TODO: handle precision loss, make policy for precision mode
// TODO: mathematical operations
// TODO: add enum type (and mappings) for physical representations: lenght, mass, etc

// base power ratios
using ratio_pow_zero = std::ratio<0, 1>;
using ratio_pow_one = std::ratio<1, 1>;

template <typename ratio_l, typename ratio_r>
struct add_ratios;

// different denominators
template <std::intmax_t num_l, std::intmax_t den_l,
        std::intmax_t num_r, std::intmax_t den_r>
struct add_ratios<std::ratio<num_l, den_l>,
        std::ratio<num_r, den_r>>
{
    using type = typename std::ratio<num_l * den_r + num_r * den_l, den_l * den_r>::type;
};

// same denominators
template <std::intmax_t num_l, std::intmax_t num_r,
        std::intmax_t den_common>
struct add_ratios<std::ratio<num_l, den_common>,
        std::ratio<num_r, den_common>>
{
    using type = typename std::ratio<num_l + num_r, den_common>::type;
};

template <typename ratio_l, typename ratio_r>
using add_ratios_t = typename add_ratios<ratio_l, ratio_r>::type;

template <typename ratio_l, typename ratio_r>
struct subtract_ratios;

// different denominators
template <std::intmax_t num_l, std::intmax_t den_l,
        std::intmax_t num_r, std::intmax_t den_r>
struct subtract_ratios<std::ratio<num_l, den_l>,
        std::ratio<num_r, den_r>>
{
    using type = typename std::ratio<num_l* den_r - num_r * den_l, den_l* den_r>::type;
};

// same denominators
template <std::intmax_t num_l, std::intmax_t num_r,
        std::intmax_t den_common>
struct subtract_ratios<std::ratio<num_l, den_common>,
        std::ratio<num_r, den_common>>
{
    using type = typename std::ratio<num_l - num_r, den_common>::type;
};

template <typename ratio_l, typename ratio_r>
using subtract_ratios_t = typename subtract_ratios<ratio_l, ratio_r>::type;

// TODO: static assert all the ratios' denums are non zero
template <typename ratio_pow_second = ratio_pow_zero,
        typename ratio_pow_metre = ratio_pow_zero,
        typename ratio_pow_kilogram = ratio_pow_zero,
        typename ratio_pow_ampere = ratio_pow_zero,
        typename ratio_pow_kelvin = ratio_pow_zero,
        typename ratio_pow_mole = ratio_pow_zero,
        typename ratio_pow_candela = ratio_pow_zero
>
struct si_descriptor_type
{
    using second_pow = ratio_pow_second;
    using metre_pow = ratio_pow_metre;
    using kilogram_pow = ratio_pow_kilogram;
    using ampere_pow = ratio_pow_ampere;
    using kelvin_pow = ratio_pow_kelvin;
    using mole_pow = ratio_pow_mole;
    using candela_pow = ratio_pow_candela;
};

template <typename si_descriptor_t_l, typename si_descriptor_t_r>
using descriptor_multiplication_res =
si_descriptor_type<
        add_ratios_t<typename si_descriptor_t_l::second_pow, typename si_descriptor_t_r::second_pow>,
        add_ratios_t<typename si_descriptor_t_l::metre_pow, typename si_descriptor_t_r::metre_pow>,
        add_ratios_t<typename si_descriptor_t_l::kilogram_pow, typename si_descriptor_t_r::kilogram_pow>,
        add_ratios_t<typename si_descriptor_t_l::ampere_pow, typename si_descriptor_t_r::ampere_pow>,
        add_ratios_t<typename si_descriptor_t_l::kelvin_pow, typename si_descriptor_t_r::kelvin_pow>,
        add_ratios_t<typename si_descriptor_t_l::mole_pow, typename si_descriptor_t_r::mole_pow>,
        add_ratios_t<typename si_descriptor_t_l::candela_pow, typename si_descriptor_t_r::candela_pow>
>;

template <typename si_descriptor_t_l, typename si_descriptor_t_r>
using descriptor_subtraction_res =
si_descriptor_type<
        subtract_ratios_t<typename si_descriptor_t_l::second_pow, typename si_descriptor_t_r::second_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::metre_pow, typename si_descriptor_t_r::metre_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::kilogram_pow, typename si_descriptor_t_r::kilogram_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::ampere_pow, typename si_descriptor_t_r::ampere_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::kelvin_pow, typename si_descriptor_t_r::kelvin_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::mole_pow, typename si_descriptor_t_r::mole_pow>,
        subtract_ratios_t<typename si_descriptor_t_l::candela_pow, typename si_descriptor_t_r::candela_pow>
>;

// dummy class which does nothing on conversion
template <typename value_type>
struct non_converting_base_si
{
    // from given unit to base SI equivalent
    constexpr value_type from(value_type val)
    {
        return val;
    }

    // from base SI equivalent to custom unit
    constexpr value_type to(value_type val)
    {
        return val;
    }
};

// class to map types
template <typename value_t = long double>
struct map_value_type
{
    using type = value_t;
};

// mapping unit tag to SI descriptor
template <typename unit_tag>
struct map_unit_tag_si_descriptor : map_value_type<void> {};

// mapping unit tag to conversion ratio
template <typename unit_tag, typename value_t = long double>
struct map_unit_tag_conversion : map_value_type<non_converting_base_si<value_t>> {};

// convenience type
template <typename unit_tag>
using si_descriptor_from_tag_t = typename map_unit_tag_si_descriptor<unit_tag>::type;

template <typename unit_tag>
using conversion_ratio_from_tag_t = typename map_unit_tag_conversion<unit_tag>::type;

using val_ratio = std::pair<std::intmax_t, std::intmax_t>;

// TODO: reduce to lowest term?
template <std::intmax_t num, std::intmax_t den>
constexpr val_ratio get_val_ratio(std::ratio<num, den>)
{
    return val_ratio{ num, den };
}

// public API type. Might be changed to hash or idk in the future
struct si_descriptor_value
{
    val_ratio second_pow = get_val_ratio(ratio_pow_zero()),
            metre_pow = get_val_ratio(ratio_pow_zero()),
            kilogram_pow = get_val_ratio(ratio_pow_zero()),
            ampere_pow = get_val_ratio(ratio_pow_zero()),
            kelvin_pow = get_val_ratio(ratio_pow_zero()),
            mole_pow = get_val_ratio(ratio_pow_zero()),
            candela_pow = get_val_ratio(ratio_pow_zero());
};


// TODO: handle equivalent ratios
constexpr bool operator==(const si_descriptor_value& lv,
                          const si_descriptor_value& rv)
{
    return lv.second_pow == rv.second_pow &&
           lv.metre_pow == rv.metre_pow &&
           lv.kilogram_pow == rv.kilogram_pow &&
           lv.ampere_pow == rv.ampere_pow &&
           lv.kelvin_pow == rv.kelvin_pow &&
           lv.mole_pow == rv.mole_pow &&
           lv.candela_pow == rv.candela_pow;
}

template <typename si_descriptor_t>
constexpr si_descriptor_value get_si_descriptor_value(si_descriptor_t)
{
    return si_descriptor_value{ get_val_ratio(typename si_descriptor_t::second_pow()),
                                get_val_ratio(typename si_descriptor_t::metre_pow()),
                                get_val_ratio(typename si_descriptor_t::kilogram_pow()),
                                get_val_ratio(typename si_descriptor_t::ampere_pow()),
                                get_val_ratio(typename si_descriptor_t::kelvin_pow()),
                                get_val_ratio(typename si_descriptor_t::mole_pow()),
                                get_val_ratio(typename si_descriptor_t::candela_pow())
    };
}


template <typename unit_t_l, typename unit_t_r>
constexpr bool units_same_value_type_v(unit_t_l, unit_t_r)
{
    return std::is_same_v<typename unit_t_l::value_type, typename unit_t_r::value_type>;
};


// check units for compatibility: they must have equal value_type and si_descriptor
template <typename unit_t_l, typename unit_t_r>
constexpr bool units_compatible_v(unit_t_l, unit_t_r)
{
    return std::is_same_v<typename unit_t_l::value_type, typename unit_t_r::value_type> &&
           unit_t_l::descriptor_value == unit_t_r::descriptor_value;
}


// TODO: copy constructor? may it throw? if may, it can not be the base class?
template <typename value_t = long double>
class unit_unknown
{
public:

    using value_type = value_t;

    // TODO: delete this method?
    constexpr value_type raw_value() const
    {
        return value_base_si_units_;
    }

    constexpr si_descriptor_value si_descriptor() const
    {
        return sdv_;
    }

    // create from basic SI units value
    constexpr static unit_unknown<value_type> create(value_type value,
                                                     const si_descriptor_value& si_descriptor)
    {
        return unit_unknown<value_t>(value, si_descriptor);
    }

protected:

    constexpr unit_unknown(value_type base_value,
                           const si_descriptor_value& si_descriptor)
            : value_base_si_units_(base_value),
              sdv_(si_descriptor)
    {}

    value_type value_base_si_units_;
    const si_descriptor_value sdv_;
};

template <typename value_t>
constexpr unit_unknown<value_t> create_unit_value(value_t value,
                                                  const si_descriptor_value& si_descriptor)
{
    return unit_unknown<value_t>::create(value, si_descriptor);
}

// tag that shows that unit construction happens without conversion of input value
struct non_converting {};

// callable convert_ratio_t type
// TODO: remove inheritence?
// TODO: allow non-default constructibe convert_ratio_t object?
// TODO: comparison operators
// TODO: comparison with tolerance?
template <typename si_descriptor_t,
        typename convert_ratio_t,
        typename value_t = long double>
class unit_type : protected unit_unknown<value_t>
{
    using base = unit_unknown<value_t>;
public:
    using value_type = value_t;
    using base::raw_value;

    using si_descriptor_type = si_descriptor_t;
    using convert_ratio_type = convert_ratio_t;
    constexpr static si_descriptor_value descriptor_value =
            get_si_descriptor_value(si_descriptor_t());

    // constructor converts given value to basic SI equivalent
    constexpr explicit unit_type(value_t from_value)
            : base(convert_ratio_t().from(from_value), descriptor_value)
    {}

    // initialize without converting
    constexpr explicit unit_type(value_t raw_value, non_converting)
            : base(raw_value, descriptor_value)
    {}

    constexpr unit_type()
            : unit_type(0.0L)
    {}

    constexpr value_type value() const
    {
        return convert_ratio_t().to(base::value_base_si_units_);
    }
};

template <typename unit_tag, typename value_type = long double>
using unit_from_tag_t = unit_type<si_descriptor_from_tag_t<unit_tag>,
        conversion_ratio_from_tag_t<unit_tag>,
        value_type>;

// non-unit operations

template <typename by_t,
        typename si_descriptor_t,
        typename convert_ratio_t,
        typename value_t = long double,
        typename = std::enable_if_t<std::is_fundamental_v<by_t>>>
constexpr unit_type<si_descriptor_t, convert_ratio_t, value_t>
operator*(unit_type<si_descriptor_t, convert_ratio_t, value_t> unit, by_t by)
{
    return unit_type<si_descriptor_t, convert_ratio_t, value_t>(unit.raw_value() * by, non_converting());
}

// TODO: check division by zero?
template <typename by_t,
        typename si_descriptor_t,
        typename convert_ratio_t,
        typename value_t = long double,
        typename = std::enable_if_t<std::is_fundamental_v<by_t>>>
constexpr unit_type<si_descriptor_t, convert_ratio_t, value_t>
operator/(unit_type<si_descriptor_t, convert_ratio_t, value_t> unit, by_t by)
{
    return unit_type<si_descriptor_t, convert_ratio_t, value_t>(unit.raw_value() / by, non_converting());
}

// unit operators

// TODO: use default SI type on return?
template <typename unit_t_l, typename unit_t_r,
        typename = std::enable_if_t<units_compatible_v(unit_t_l(), unit_t_r())>>
constexpr unit_t_l operator+(const unit_t_l& ul, const unit_t_r& ur)
{
    return unit_t_l(ul.raw_value() + ur.raw_value(), non_converting());
}

// TODO: use default SI type on return?
template <typename unit_t_l, typename unit_t_r// ,
        // typename = std::enable_if_t<units_compatible_v(unit_t_l(), unit_t_r())>
>
constexpr unit_t_l operator-(const unit_t_l& ul, const unit_t_r& ur)
{
    return unit_t_l(ul.raw_value() - ur.raw_value(), non_converting());
}

// returns non_converting
template <typename desriptor_t_l, typename convert_ratio_t_l,
        typename desriptor_t_r, typename convert_ratio_t_r,
        typename value_type>
constexpr auto operator*(const unit_type<desriptor_t_l, convert_ratio_t_l, value_type>& lv,
                         const unit_type<desriptor_t_r, convert_ratio_t_r, value_type>& rv)
{
    return unit_type<
            descriptor_multiplication_res<desriptor_t_l, desriptor_t_r>,
            non_converting_base_si<value_type>,
            value_type
    >(lv.raw_value() * rv.raw_value(), non_converting());
}

// returns non_converting
// TODO: check non null?
template <typename desriptor_t_l, typename convert_ratio_t_l,
        typename desriptor_t_r, typename convert_ratio_t_r,
        typename value_type>
constexpr auto operator/(const unit_type<desriptor_t_l, convert_ratio_t_l, value_type>& lv,
                         const unit_type<desriptor_t_r, convert_ratio_t_r, value_type>& rv)
{
    return unit_type<
            descriptor_subtraction_res<desriptor_t_l, desriptor_t_r>,
            non_converting_base_si<value_type>,
            value_type
    >(lv.raw_value() / rv.raw_value(), non_converting());
}

// base SI units descriptor types
using si_unitless_descriptor_t = si_descriptor_type<>;
using si_second_descriptor_t = si_descriptor_type<ratio_pow_one>;
using si_metre_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_one>;
using si_kilogram_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_one>;
using si_ampere_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_one>;
using si_kelvin_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_one>;
using si_mole_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_one>;
using si_candela_descriptor_t = si_descriptor_type<ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_zero,
        ratio_pow_one>;

// base SI units descriptor values
constexpr si_descriptor_value si_unitless_descriptor_v = get_si_descriptor_value(si_unitless_descriptor_t());
constexpr si_descriptor_value si_second_descriptor_v = get_si_descriptor_value(si_second_descriptor_t());
constexpr si_descriptor_value si_metre_descriptor_v = get_si_descriptor_value(si_metre_descriptor_t());
constexpr si_descriptor_value si_kilogram_descriptor_v = get_si_descriptor_value(si_kilogram_descriptor_t());
constexpr si_descriptor_value si_ampere_descriptor_v = get_si_descriptor_value(si_ampere_descriptor_t());
constexpr si_descriptor_value si_kelvin_descriptor_v = get_si_descriptor_value(si_kelvin_descriptor_t());
constexpr si_descriptor_value si_mole_descriptor_v = get_si_descriptor_value(si_mole_descriptor_t());
constexpr si_descriptor_value si_candela_descriptor_v = get_si_descriptor_value(si_candela_descriptor_t());

// tags
struct tag_unitless;
struct tag_second;
struct tag_metre;
struct tag_kilogram;
struct tag_ampere;
struct tag_kelvin;
struct tag_mole;
struct tag_candela;

// register tags
template <> struct map_unit_tag_si_descriptor<tag_unitless> : map_value_type<si_unitless_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_second> : map_value_type<si_second_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_metre> : map_value_type<si_metre_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_kilogram> : map_value_type<si_kilogram_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_ampere> : map_value_type<si_ampere_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_kelvin> : map_value_type<si_kelvin_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_mole> : map_value_type<si_mole_descriptor_t> {};
template <> struct map_unit_tag_si_descriptor<tag_candela> : map_value_type<si_candela_descriptor_t> {};

// TODO: add template value_type?
// predefined unit types
using unitless = unit_from_tag_t<tag_unitless>;
using second = unit_from_tag_t<tag_second>;
using metre = unit_from_tag_t<tag_metre>;
using kilogram = unit_from_tag_t<tag_kilogram>;
using ampere = unit_from_tag_t<tag_ampere>;
using kelvin = unit_from_tag_t<tag_kelvin>;
using mole = unit_from_tag_t<tag_mole>;
using candela = unit_from_tag_t<tag_candela>;

namespace literals
{
    constexpr second operator""_s(long double val)
    {
        return second(val);
    }
    constexpr metre operator""_m(long double val)
    {
        return metre(val);
    }
}
