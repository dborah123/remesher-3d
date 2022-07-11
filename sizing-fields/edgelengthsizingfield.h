#ifndef FLUX_EDGELENGTH_SIZINGFIELD_H
#define FLUX_EDGELENGTH_SIZINGFIELD_H

namespace flux {

class EdgelengthSizingField {
public:

EdgelengthSizingField(double edgelength);

double operator()(const double *x) const;

private:
double _edgelength;
};
} // flux

#endif