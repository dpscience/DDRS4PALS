#ifndef DMEDIANFILTER_H
#define DMEDIANFILTER_H

#include "DLib/DThirdParty/MedianFilter/medflt.h"

template <typename DItem> class DMediator {
    const int m_N;
    DItem* m_data;
    int* m_pos;
    int* m_heap;
    int* m_allocatedHeap;
    int m_idx;
    int m_minCt;
    int m_maxCt;

public:
    DMediator(int nItems) :
        m_N(nItems) {
        m_data = new DItem[nItems];
        m_pos = new int[nItems];
        m_allocatedHeap = new int[nItems];
        m_heap = m_allocatedHeap + (nItems/2);
        m_minCt = m_maxCt = m_idx = 0;

        while (nItems--) {
            m_pos[nItems] = ((nItems + 1)/2)*((nItems & 1) ? -1 : 1);
            m_heap[m_pos[nItems]] = nItems;
        }
    }

    virtual ~DMediator() {
        delete[] m_data;
        delete[] m_pos;
        delete[] m_allocatedHeap;
    }

    void insert(const DItem& v) {
        const int p = m_pos[m_idx];
        const DItem old = m_data[m_idx];
        m_data[m_idx] = v;
        m_idx = (m_idx+1)%m_N;

        if (p > 0) {
            if (m_minCt < (m_N-1)/2) {
                m_minCt ++;
            }
            else if (v > old) {
                minSortDown( p );
                return;
            }

            if (minSortUp(p) && mmCmpExch(0, -1)) {
                maxSortDown(-1);
            }
        }
        else if (p < 0) {
            if (m_maxCt < m_N/2) {
                m_maxCt++;
            }
            else if (v < old) {
                maxSortDown( p );
                return;
            }

            if (maxSortUp(p) && m_minCt && mmCmpExch(1, 0)) {
                minSortDown(1);
            }
        }
        else {
            if (m_maxCt && maxSortUp(-1)) {
                maxSortDown( -1 );
            }

            if (m_minCt && minSortUp(1)) {
                minSortDown(1);
            }
        }
    }

    DItem getMedian() {
        DItem v = m_data[m_heap[0]];

        if (m_minCt < m_maxCt) {
            v = (v + m_data[m_heap[-1]])/2;
        }

        return v;
    }

private:
    int mmexchange(const int i, const int j)
    {
        int t = m_heap[i];
        m_heap[i] = m_heap[j];
        m_heap[j] = t;
        m_pos[m_heap[i]] = i;
        m_pos[m_heap[j]] = j;

        return 1;
    }

    void minSortDown(int i) {
        for (i*=2; i <=m_minCt; i*=2) {
            if (i < m_minCt && mmless(i+1, i)) {
                ++i;
            }

            if (!mmCmpExch(i, i/2)) {
                break;
            }
        }
    }

    void maxSortDown(int i) {
        for (i*=2; i >= -m_maxCt; i*=2) {
            if (i > -m_maxCt && mmless(i, i-1)) {
                --i;
            }

            if (!mmCmpExch(i/2, i)) {
                break;
            }
        }
    }

    inline int mmless(const int i,const int j) {
        return (m_data[m_heap[i]] < m_data[m_heap[j]]);
    }

    inline int mmCmpExch(const int i,const int j) {
        return (mmless(i, j) && mmexchange(i, j));
    }

    inline int minSortUp(int i) {
        while (i > 0 && mmCmpExch(i, i/2)) {
            i /= 2;
        }

        return (i == 0);
    }

    inline int maxSortUp(int i) {
        while (i < 0 && mmCmpExch(i/2, i)) {
            i /= 2;
        }

        return (i==0);
    }
};

class DMedianFilter
{
    DMedianFilter();
    virtual ~DMedianFilter();

public:
    static bool apply(float *data_1d, float *data_1d_source, int size, int windowSize);
};

#endif // DMEDIANFILTER_H
