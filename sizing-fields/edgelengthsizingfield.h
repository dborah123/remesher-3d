#ifndef FLUX_EDGELENGTH_SIZINGFIELD_H
#define FLUX_EDGELENGTH_SIZINGFIELD_H
#include "size.h"

namespace flux {

class EdgelengthSizingField : public SizingField<3> {
public:

EdgelengthSizingField(double edgelength);

double operator()(const double *x) const;

private:
double _edgelength;
};
} // flux

#endif