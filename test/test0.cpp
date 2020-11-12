
#include <libtsu/libtsu_public.h>

int main()
{
    constexpr val_ratio v1 = get_val_ratio(std::ratio<1, 1>{});
    constexpr unit_unknown<> un1 = create_unit_value(36.0L, si_metre_descriptor_v);

    sizeof(si_descriptor_value);
    sizeof(metre);
    constexpr metre m1{ 1 };
    static_assert(metre(1).value() == 1.0L, "");
    // static_assert(metre(2) == metre(1) + metre(1), "");
    constexpr metre m2 = (metre(1) + metre(1) + metre(1)) * 2;

    static_assert(get_val_ratio(std::ratio<1, 1>{}) == get_val_ratio(std::ratio<1, 1>{}),
                  "Unexpected result");

    static_assert(std::is_same_v<si_unitless_descriptor_t, si_descriptor_from_tag_t<tag_unitless>>,
                  "Invalid descriptor from tag_unitless");
    static_assert(std::is_same_v<si_second_descriptor_t, si_descriptor_from_tag_t<tag_second>>,
                  "Invalid descriptor from tag_second");
    static_assert(std::is_same_v<si_metre_descriptor_t, si_descriptor_from_tag_t<tag_metre>>,
                  "Invalid descriptor from tag_metre");
    static_assert(std::is_same_v<si_kilogram_descriptor_t, si_descriptor_from_tag_t<tag_kilogram>>,
                  "Invalid descriptor from tag_kilogram");
    static_assert(std::is_same_v<si_ampere_descriptor_t, si_descriptor_from_tag_t<tag_ampere>>,
                  "Invalid descriptor from tag_ampere");
    static_assert(std::is_same_v<si_kelvin_descriptor_t, si_descriptor_from_tag_t<tag_kelvin>>,
                  "Invalid descriptor from tag_kelvin");
    static_assert(std::is_same_v<si_mole_descriptor_t, si_descriptor_from_tag_t<tag_mole>>,
                  "Invalid descriptor from tag_mole");
    static_assert(std::is_same_v<si_candela_descriptor_t, si_descriptor_from_tag_t<tag_candela>>,
                  "Invalid descriptor from tag_candela");

    static_assert(si_unitless_descriptor_v == unitless::descriptor_value,
                  "Descriptor values don't match for unitless");

    static_assert(units_compatible_v(unitless(), unitless()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(second(), second()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(metre(), metre()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(kilogram(), kilogram()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(ampere(), ampere()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(kelvin(), kelvin()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(mole(), mole()), "Unexpected result for compatibility");
    static_assert(units_compatible_v(candela(), candela()), "Unexpected result for compatibility");

    // for multiplication testing
    using si_metre_square_descr = si_descriptor_type<ratio_pow_zero, std::ratio<2, 1>>;

    constexpr auto msquare = metre(2) * metre(2);
    constexpr auto speed = metre(120) / second(10);
    constexpr auto acceleration = speed / second(2);

    constexpr metre distance = acceleration * second(2) * second(2);

    return 0;
}