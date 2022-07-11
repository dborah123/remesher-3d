#include "edgelengthsizingfield.h"

namespace flux {

EdgelengthSizingField::EdgelengthSizingField(double edgelength) :
_edgelength(edgelength) {  }

double
EdgelengthSizingField::operator()(const double *x) const {
    return _edgelength;
}


}