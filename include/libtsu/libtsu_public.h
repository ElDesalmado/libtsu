#pragma once

#include <type_traits>
#include <cstdint>
#include <ratio>
#include <utility>

#if __cplusplus < 201703L
namespace std
{
    template<class T, class U>
    constexpr bool is_same_v = is_same<T, U>::value;

    template<class T>
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

namespace tsu
{
    // public types for mapping

    // TODO: error if type is not registered
    // class to map types
    template<typename value_t>
    struct map_value_type
    {
//        static_assert(!std::is_same_v<value_t, tag_value_not_found>,
//                "map_value_type: value for key is not registered");
        using type = value_t;
    };

    struct tag_unit_not_registered;

    // mapping unit tag to SI descriptor
    template<typename unit_tag>
    struct map_unit_tag_to_descriptor // : map_value_type<tag_value_not_found>
    {
        // TODO: find out the way to show an error
        // this works only if object is being instantiated
        static_assert(!std::is_same_v<unit_tag, tag_unit_not_registered>,
                "map_value_type: value for key is not registered");

        // this actually works, but error is not clear
        // using type = tag_unit_not_registered;
    };

    // convenience type
    template<typename unit_tag>
    using descriptor_from_unit_tag_t = typename map_unit_tag_to_descriptor<unit_tag>::type;

    // dummy class which does nothing on conversion
    template<typename value_type>
    struct conversion_none
    {
        // from given unit to base SI equivalent
        constexpr value_type to_basic_unit(value_type val)
        {
            return val;
        }

        // from base SI equivalent to custom unit
        constexpr value_type from_basic_unit(value_type val)
        {
            return val;
        }
    };

    // TODO: remove default conversion? It is convenient...
// mapping unit tag to conversion ratio
    template<typename unit_tag, typename value_t = long double>
    struct map_unit_tag_to_convertor : map_value_type<conversion_none<value_t>>
    {
        // TODO: find out the way to show an error
        // this works only if object is being instantiated
//        static_assert(!std::is_same_v<unit_tag, tag_unit_not_registered>,
//                      "map_value_type: value for key is not registered");

        // this actually works, but error is not clear
        // using type = tag_unit_not_registered;
    };

    template<typename unit_tag>
    using convertor_from_unit_tag_t = typename map_unit_tag_to_convertor<unit_tag>::type;

// check units for compatibility: they must have equal value_type and si_descriptor
    template<typename unit_t_l, typename unit_t_r>
    constexpr bool units_compatible_v(unit_t_l, unit_t_r)
    {
        return std::is_same_v<typename unit_t_l::value_type, typename unit_t_r::value_type> &&
               std::is_same_v<typename unit_t_l::descriptor_type, typename unit_t_r::descriptor_type>;
    }

    namespace impl
    {
        /*
    * TODO: descriptor pow res
    * TODO: descriptor sqrt res
    * TODO: descriptor exp result?
    */

        // TODO: error if no implementation is defined for given descriptors
        /**
         * abstraction typename to get resulting descriptor after units multiplication
         * @tparam descriptor_t_l - descriptor of left unit
         * @tparam descriptor_t_r - descriptor of right unit
         */
        template<typename descriptor_t_l, typename descriptor_t_r>
        struct res_descriptors_multiplication;

        template<typename descriptor_t_l, typename descriptor_t_r>
        using res_descriptors_multiplication_t =
        typename res_descriptors_multiplication<descriptor_t_l, descriptor_t_r>::type;

        // TODO: error if no implementation is defined for given descriptors
        /**
         * abstraction typename to get resulting descriptor after units division
         * @tparam descriptor_t_l
         * @tparam descriptor_t_r
         * */
        template<typename descriptor_t_l, typename descriptor_t_r>
        struct res_descriptors_division;

        template<typename descriptor_t_l, typename descriptor_t_r>
        using res_descriptors_division_t =
        typename res_descriptors_division<descriptor_t_l, descriptor_t_r>::type;

        // TODO: make struct
        template<typename unit_t_l, typename unit_t_r>
        constexpr bool units_same_value_type_v(unit_t_l, unit_t_r)
        {
            return std::is_same_v<typename unit_t_l::value_type, typename unit_t_r::value_type>;
        };

    }

    // tag that shows that unit construction happens without conversion of input value
    struct non_converting
    {
    };

// callable convertor_t type
// TODO: remove inheritence?
// TODO: allow non-default constructibe convertor_t object?
// TODO: comparison operators
// TODO: comparison with tolerance?
    template<typename descriptor_t,
            typename convertor_t,
            typename value_t = long double>
    class unit_type
    {
    public:
        using value_type = value_t;

        using descriptor_type = descriptor_t;
        using convertor_type = convertor_t;

        // TODO: do I need this?
//        constexpr static descriptor_t descriptor_value =
//                get_si_descriptor_value(descriptor_t());

        // constructor converts given value to basic SI equivalent
        constexpr explicit unit_type(value_t from_value)
                : base_unit_value_(convertor_t().to_basic_unit(from_value))
        {}

        // initialize without converting
        constexpr explicit unit_type(value_t raw_value, non_converting)
                : base_unit_value_(raw_value)
        {}

        constexpr unit_type()
                : unit_type(0.0L)
        {}

        constexpr value_type value() const
        {
            return convertor_t().from_basic_unit(base_unit_value_);
        }

        constexpr value_type raw_value() const
        {
            return base_unit_value_;
        }

    private:
        value_type base_unit_value_;
    };

    template<typename unit_tag, typename value_type = long double>
    using unit_from_tag_t = unit_type<descriptor_from_unit_tag_t<unit_tag>,
            convertor_from_unit_tag_t<unit_tag>,
            value_type>;

// non-unit operations

    template<typename by_t,
            typename descriptor_t,
            typename convertor_t,
            typename value_t = long double,
            typename = std::enable_if_t<std::is_fundamental_v<by_t>>>
    constexpr unit_type<descriptor_t, convertor_t, value_t>
    operator*(unit_type<descriptor_t, convertor_t, value_t> unit, by_t by)
    {
        return unit_type<descriptor_t, convertor_t, value_t>(unit.raw_value() * by, non_converting());
    }

// TODO: check division by zero?
    template<typename by_t,
            typename descriptor_t,
            typename convertor_t,
            typename value_t = long double,
            typename = std::enable_if_t<std::is_fundamental_v<by_t>>>
    constexpr unit_type<descriptor_t, convertor_t, value_t>
    operator/(unit_type<descriptor_t, convertor_t, value_t> unit, by_t by)
    {
        return unit_type<descriptor_t, convertor_t, value_t>(unit.raw_value() / by, non_converting());
    }

// unit operators

// TODO: use default SI type on return?
    template<typename unit_t_l, typename unit_t_r,
            typename = std::enable_if_t<units_compatible_v(unit_t_l(), unit_t_r())>>
    constexpr unit_t_l operator+(const unit_t_l &ul, const unit_t_r &ur)
    {
        return unit_t_l(ul.raw_value() + ur.raw_value(), non_converting());
    }

// TODO: use default SI type on return?
    template<typename unit_t_l, typename unit_t_r// ,
            // typename = std::enable_if_t<units_compatible_v(unit_t_l(), unit_t_r())>
    >
    constexpr unit_t_l operator-(const unit_t_l &ul, const unit_t_r &ur)
    {
        return unit_t_l(ul.raw_value() - ur.raw_value(), non_converting());
    }

// returns non_converting
    template<typename descriptor_t_l, typename convert_ratio_t_l,
            typename descriptor_t_r, typename convert_ratio_t_r,
            typename value_type>
    constexpr auto operator*(const unit_type<descriptor_t_l, convert_ratio_t_l, value_type> &lv,
                             const unit_type<descriptor_t_r, convert_ratio_t_r, value_type> &rv)
    {
        return unit_type<
                impl::res_descriptors_multiplication_t<descriptor_t_l, descriptor_t_r>,
                conversion_none<value_type>,
                value_type
        >(lv.raw_value() * rv.raw_value(), non_converting());
    }

// returns non_converting
// TODO: check non null?
    template<typename descriptor_t_l, typename convert_ratio_t_l,
            typename descriptor_t_r, typename convert_ratio_t_r,
            typename value_type>
    constexpr auto operator/(const unit_type<descriptor_t_l, convert_ratio_t_l, value_type> &lv,
                             const unit_type<descriptor_t_r, convert_ratio_t_r, value_type> &rv)
    {
        return unit_type<
                impl::res_descriptors_division_t<descriptor_t_l, descriptor_t_r>,
                conversion_none<value_type>,
                value_type
        >(lv.raw_value() / rv.raw_value(), non_converting());
    }


// base power ratios
    using ratio_pow_zero = std::ratio<0, 1>;
    using ratio_pow_one = std::ratio<1, 1>;

    template<typename ratio_l, typename ratio_r>
    struct add_ratios;

// different denominators
    template<std::intmax_t num_l, std::intmax_t den_l,
            std::intmax_t num_r, std::intmax_t den_r>
    struct add_ratios<std::ratio<num_l, den_l>,
            std::ratio<num_r, den_r>>
    {
        using type = typename std::ratio<num_l * den_r + num_r * den_l, den_l * den_r>::type;
    };

// same denominators
    template<std::intmax_t num_l, std::intmax_t num_r,
            std::intmax_t den_common>
    struct add_ratios<std::ratio<num_l, den_common>,
            std::ratio<num_r, den_common>>
    {
        using type = typename std::ratio<num_l + num_r, den_common>::type;
    };

    template<typename ratio_l, typename ratio_r>
    using add_ratios_t = typename add_ratios<ratio_l, ratio_r>::type;

    template<typename ratio_l, typename ratio_r>
    struct subtract_ratios;

// different denominators
    template<std::intmax_t num_l, std::intmax_t den_l,
            std::intmax_t num_r, std::intmax_t den_r>
    struct subtract_ratios<std::ratio<num_l, den_l>,
            std::ratio<num_r, den_r>>
    {
        using type = typename std::ratio<num_l * den_r - num_r * den_l, den_l * den_r>::type;
    };

// same denominators
    template<std::intmax_t num_l, std::intmax_t num_r,
            std::intmax_t den_common>
    struct subtract_ratios<std::ratio<num_l, den_common>,
            std::ratio<num_r, den_common>>
    {
        using type = typename std::ratio<num_l - num_r, den_common>::type;
    };

    template<typename ratio_l, typename ratio_r>
    using subtract_ratios_t = typename subtract_ratios<ratio_l, ratio_r>::type;

// TODO: move to another place
// TODO: static assert all the ratios' denums are non zero
    template<typename ratio_pow_second = ratio_pow_zero,
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

    template<typename ... t_args_l,
            typename ... t_args_r>
    struct impl::res_descriptors_multiplication<si_descriptor_type<t_args_l...>, si_descriptor_type<t_args_r...>>
    {
        using type = si_descriptor_type<
                add_ratios_t<typename si_descriptor_type<t_args_l...>::second_pow, typename si_descriptor_type<t_args_r...>::second_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::metre_pow, typename si_descriptor_type<t_args_r...>::metre_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::kilogram_pow, typename si_descriptor_type<t_args_r...>::kilogram_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::ampere_pow, typename si_descriptor_type<t_args_r...>::ampere_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::kelvin_pow, typename si_descriptor_type<t_args_r...>::kelvin_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::mole_pow, typename si_descriptor_type<t_args_r...>::mole_pow>,
                add_ratios_t<typename si_descriptor_type<t_args_l...>::candela_pow, typename si_descriptor_type<t_args_r...>::candela_pow>>;
    };

    template<typename ... t_args_l, typename ... t_args_r>
    struct impl::res_descriptors_division<si_descriptor_type<t_args_l...>, si_descriptor_type<t_args_r...>>
    {
        using type = si_descriptor_type<
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::second_pow, typename si_descriptor_type<t_args_r...>::second_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::metre_pow, typename si_descriptor_type<t_args_r...>::metre_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::kilogram_pow, typename si_descriptor_type<t_args_r...>::kilogram_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::ampere_pow, typename si_descriptor_type<t_args_r...>::ampere_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::kelvin_pow, typename si_descriptor_type<t_args_r...>::kelvin_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::mole_pow, typename si_descriptor_type<t_args_r...>::mole_pow>,
                subtract_ratios_t<typename si_descriptor_type<t_args_l...>::candela_pow, typename si_descriptor_type<t_args_r...>::candela_pow>
        >;
    };

    using val_ratio = std::pair<std::intmax_t, std::intmax_t>;

// TODO: reduce to lowest term?
    template<std::intmax_t num, std::intmax_t den>
    constexpr val_ratio get_val_ratio(std::ratio<num, den>)
    {
        return val_ratio{num, den};
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
    constexpr bool operator==(const si_descriptor_value &lv,
                              const si_descriptor_value &rv)
    {
        return lv.second_pow == rv.second_pow &&
               lv.metre_pow == rv.metre_pow &&
               lv.kilogram_pow == rv.kilogram_pow &&
               lv.ampere_pow == rv.ampere_pow &&
               lv.kelvin_pow == rv.kelvin_pow &&
               lv.mole_pow == rv.mole_pow &&
               lv.candela_pow == rv.candela_pow;
    }

    template<typename si_descriptor_t>
    constexpr si_descriptor_value get_si_descriptor_value(si_descriptor_t)
    {
        return si_descriptor_value{get_val_ratio(typename si_descriptor_t::second_pow()),
                                   get_val_ratio(typename si_descriptor_t::metre_pow()),
                                   get_val_ratio(typename si_descriptor_t::kilogram_pow()),
                                   get_val_ratio(typename si_descriptor_t::ampere_pow()),
                                   get_val_ratio(typename si_descriptor_t::kelvin_pow()),
                                   get_val_ratio(typename si_descriptor_t::mole_pow()),
                                   get_val_ratio(typename si_descriptor_t::candela_pow())
        };
    }


// TODO: copy constructor? may it throw? if may, it can not be the base class?
    template<typename value_t = long double>
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
                                                         const si_descriptor_value &si_descriptor)
        {
            return unit_unknown<value_t>(value, si_descriptor);
        }

    protected:

        constexpr unit_unknown(value_type base_value,
                               const si_descriptor_value &si_descriptor)
                : value_base_si_units_(base_value),
                  sdv_(si_descriptor)
        {}

        value_type value_base_si_units_;
        const si_descriptor_value sdv_;
    };

    template<typename value_t>
    constexpr unit_unknown<value_t> create_unit_value(value_t value,
                                                      const si_descriptor_value &si_descriptor)
    {
        return unit_unknown<value_t>::create(value, si_descriptor);
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
    template<> struct map_unit_tag_to_descriptor<tag_unitless> : map_value_type<si_unitless_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_second> : map_value_type<si_second_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_metre> : map_value_type<si_metre_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_kilogram> : map_value_type<si_kilogram_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_ampere> : map_value_type<si_ampere_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_kelvin> : map_value_type<si_kelvin_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_mole> : map_value_type<si_mole_descriptor_t>
    {
    };
    template<> struct map_unit_tag_to_descriptor<tag_candela> : map_value_type<si_candela_descriptor_t>
    {
    };

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
        constexpr second operator ""_s(long double val)
        {
            return second(val);
        }

        constexpr metre operator ""_m(long double val)
        {
            return metre(val);
        }
    }

}

