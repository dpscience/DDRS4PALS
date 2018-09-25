#include "dmedianfilter.h"

DMedianFilter::DMedianFilter() {}
DMedianFilter::~DMedianFilter() {}

#ifdef __DMEDIANFILTER_VARIANT_1
bool DMedianFilter::apply(float *data_1d, int size, int windowSize)
{
    if (!data_1d || size < 3 || windowSize < 3)
        return false;

    const int windowSizeHalf = windowSize/2;
    const int offset = windowSizeHalf + (windowSize%2);

    if (!autocastToInteger) {
        DMediator<float> mediator(windowSize);
        for (int i = 0 ; i < windowSizeHalf ; ++i) {
            mediator.insert(data_1d[0]);
            data_1d[i] = mediator.getMedian();
        }

        for (int i = 0 ; i < offset ; ++i) {
            mediator.insert(data_1d[i]);
        }

        const int nDec = (size - offset);
        const int nMinus1 = (size - 1);

        for (int i = 0 ; i < nDec ; ++i) {
            data_1d[i] = mediator.getMedian();
            mediator.insert(data_1d[i + offset]);
        }

        for (int i = nDec ; i < size ; ++i) {
            data_1d[i] = mediator.getMedian();
            mediator.insert(data_1d[nMinus1]);
        }
    }
    else {
        DMediator<int> mediator(windowSize);
        for (int i = 0 ; i < windowSizeHalf ; ++i) {
            mediator.insert(data_1d[0]);
            data_1d[i] = mediator.getMedian();
        }

        for (int i = 0 ; i < offset ; ++i) {
            mediator.insert(data_1d[i]);
        }

        const int nDec = (size - offset);
        const int nMinus1 = (size - 1);

        for (int i = 0 ; i < nDec ; ++i) {
            data_1d[i] = mediator.getMedian();
            mediator.insert(data_1d[i + offset]);
        }

        for (int i = nDec ; i < size ; ++i) {
            data_1d[i] = mediator.getMedian();
            mediator.insert(data_1d[nMinus1]);
        }
    }

    return true;
}
#endif

bool DMedianFilter::apply(float *data_1d, int size, int windowSize)
{
    if (!data_1d || size <= 3 || windowSize < 3)
        return false;

    TMedianFilter1D<float> filter(windowSize);

    std::vector<float> data(size);
    for ( int i = 0 ; i < size ; ++ i ) {
        data[i] = data_1d[i];
    }

    filter.Execute(data);

    for ( int i = 0 ; i < filter.Count() ; ++ i )
        data_1d[i] = filter[i];


    return true;
}


