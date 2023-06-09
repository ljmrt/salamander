#ifndef MATHUTILS_H
#define MATHUTILS_H


namespace MathUtils
{
    // normalize value to targetMinimumValue..targetMaximumValue through the equation shown here(https://stats.stackexchange.com/questions/281162/scale-a-number-between-a-range).
    //
    // @param initialValue the initial value.
    // @param initialRangeMinimumValue the lowest(minimum) value in the initial range.
    // @param initialRangeMaximumValue the highest(maximum) value in the initial range.
    // @param targetRangeMinimumValue the lowest(minimum) value in the target range.
    // @param targetRangeMaximumValue the highest(maximum) value in the target range.
    // @return normalized value.
    float normalizeValueToRanges(float initialValue, float initialRangeMinimumValue, float initialRangeMaximumValue, float targetRangeMinimumValue, float targetRangeMaximumValue);
}


#endif  // MATHUTILS_H
