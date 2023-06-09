#include <utils/MathUtils.h>


float MathUtils::normalizeValueToRanges(float initialValue, float initialRangeMinimumValue, float initialRangeMaximumValue, float targetRangeMinimumValue, float targetRangeMaximumValue)
{
    float zeroToOneNormalizedValue = ((initialValue - initialRangeMinimumValue) / (initialRangeMaximumValue - initialRangeMinimumValue));
    return (zeroToOneNormalizedValue * ((targetRangeMaximumValue - targetRangeMinimumValue) + targetRangeMinimumValue));
}
