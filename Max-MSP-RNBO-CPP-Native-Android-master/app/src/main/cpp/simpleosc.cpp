/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, -1468824490, false);
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

number samplerate() const {
    return this->sr;
}

inline number safemod(number f, number m) {
    if (m != 0) {
        Int f_trunc = (Int)(trunc(f));
        Int m_trunc = (Int)(trunc(m));

        if (f == f_trunc && m == m_trunc) {
            f = f_trunc % m_trunc;
        } else {
            if (m < 0) {
                m = -m;
            }

            if (f >= m) {
                if (f >= m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f -= m;
                }
            } else if (f <= -m) {
                if (f <= -m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f += m;
                }
            }
        }
    } else {
        f = 0.0;
    }

    return f;
}

inline number safesqrt(number num) {
    return (num > 0.0 ? rnbo_sqrt(num) : 0.0);
}

Index bufferbindchannel(const Index channel, const Index maxChannels, const int channelmode) {
    if (channelmode == 0 || channelmode == 4) {
        return maxChannels - 1 - channel % maxChannels;
    } else if (channelmode == 3) {
        return (maxChannels == 0 ? 0 : channel % maxChannels);
    } else {
        return (channel > maxChannels - 1 ? maxChannels - 1 : (channel < 0 ? 0 : channel));
    }
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    long numWraps = (long)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

SampleValue bufferbindindex(
    const SampleValue index,
    const SampleIndex start,
    const SampleIndex end,
    const int boundmode
) {
    if (boundmode == 0 || boundmode == 4) {
        if (index >= start && index < end)
            return index;
        else {
            SampleIndex length = (SampleIndex)(end - start);
            number index1 = index - start;
            number wrap = (index1 < 0 ? length * 2 - 1 + this->safemod(index1 + 1, length * 2) : this->safemod(index1, length * 2));
            return (start + wrap >= length ? length * 2 - wrap - 1 : wrap);
        }
    } else if (boundmode == 3) {
        return this->wrap(index, start, end);
    } else {
        return (index > end - 1 ? end - 1 : (index < start ? start : index));
    }
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a1 = 1. + a;
    number aa = a * a1;
    number b = 1. - a;
    number b1 = 2. - a;
    number bb = b * b1;
    number fw = -.1666667 * bb * a;
    number fx = .5 * bb * a1;
    number fy = .5 * aa * b1;
    number fz = -.1666667 * aa * b;
    return w * fw + x * fx + y * fy + z * fz;
}

inline number fastcubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number spline6interp(number a, number y0, number y1, number y2, number y3, number y4, number y5) {
    number ym2py2 = y0 + y4;
    number ym1py1 = y1 + y3;
    number y2mym2 = y4 - y0;
    number y1mym1 = y3 - y1;
    number sixthym1py1 = (number)1 / (number)6.0 * ym1py1;
    number c0 = (number)1 / (number)120.0 * ym2py2 + (number)13 / (number)60.0 * ym1py1 + (number)11 / (number)20.0 * y2;
    number c1 = (number)1 / (number)24.0 * y2mym2 + (number)5 / (number)12.0 * y1mym1;
    number c2 = (number)1 / (number)12.0 * ym2py2 + sixthym1py1 - (number)1 / (number)2.0 * y2;
    number c3 = (number)1 / (number)12.0 * y2mym2 - (number)1 / (number)6.0 * y1mym1;
    number c4 = (number)1 / (number)24.0 * ym2py2 - sixthym1py1 + (number)1 / (number)4.0 * y2;
    number c5 = (number)1 / (number)120.0 * (y5 - y0) + (number)1 / (number)24.0 * (y1 - y4) + (number)1 / (number)12.0 * (y3 - y2);
    return ((((c5 * a + c4) * a + c3) * a + c2) * a + c1) * a + c0;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

template <typename T> inline SampleValue bufferreadsample(
    T& buffer,
    const SampleValue sampleIndex,
    const Index channel,
    const SampleIndex start,
    const SampleIndex end,
    const int interp
) {
    if (sampleIndex < 0.0) {
        return 0.0;
    }

    SampleIndex truncIndex = (SampleIndex)(trunc(sampleIndex));

    if (interp == 5) {
        return buffer->getSample(channel, truncIndex);
    } else {
        number frac = sampleIndex - truncIndex;
        number wrap = end - 1;

        if (interp == 0) {
            SampleIndex index1 = (SampleIndex)(truncIndex);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            return this->linearinterp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2)
            );
        } else if (interp == 1) {
            SampleIndex index1 = (SampleIndex)(truncIndex - 1);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            SampleIndex index3 = (SampleIndex)(index2 + 1);

            if (index3 > wrap)
                index3 = start;

            SampleIndex index4 = (SampleIndex)(index3 + 1);

            if (index4 > wrap)
                index4 = start;

            return this->cubicinterp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2),
                buffer->getSample(channel, index3),
                buffer->getSample(channel, index4)
            );
        } else if (interp == 6) {
            SampleIndex index1 = (SampleIndex)(truncIndex - 1);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            SampleIndex index3 = (SampleIndex)(index2 + 1);

            if (index3 > wrap)
                index3 = start;

            SampleIndex index4 = (SampleIndex)(index3 + 1);

            if (index4 > wrap)
                index4 = start;

            return this->fastcubicinterp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2),
                buffer->getSample(channel, index3),
                buffer->getSample(channel, index4)
            );
        } else if (interp == 2) {
            SampleIndex index1 = (SampleIndex)(truncIndex - 1);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            SampleIndex index3 = (SampleIndex)(index2 + 1);

            if (index3 > wrap)
                index3 = start;

            SampleIndex index4 = (SampleIndex)(index3 + 1);

            if (index4 > wrap)
                index4 = start;

            return this->splineinterp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2),
                buffer->getSample(channel, index3),
                buffer->getSample(channel, index4)
            );
        } else if (interp == 7) {
            SampleIndex index1 = (SampleIndex)(truncIndex - 2);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            SampleIndex index3 = (SampleIndex)(index2 + 1);

            if (index3 > wrap)
                index3 = start;

            SampleIndex index4 = (SampleIndex)(index3 + 1);

            if (index4 > wrap)
                index4 = start;

            SampleIndex index5 = (SampleIndex)(index4 + 1);

            if (index5 > wrap)
                index5 = start;

            SampleIndex index6 = (SampleIndex)(index5 + 1);

            if (index6 > wrap)
                index6 = start;

            return this->spline6interp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2),
                buffer->getSample(channel, index3),
                buffer->getSample(channel, index4),
                buffer->getSample(channel, index5),
                buffer->getSample(channel, index6)
            );
        } else if (interp == 3) {
            SampleIndex index1 = (SampleIndex)(truncIndex);

            if (index1 < 0)
                index1 = wrap;

            SampleIndex index2 = (SampleIndex)(index1 + 1);

            if (index2 > wrap)
                index2 = start;

            return this->cosineinterp(
                frac,
                buffer->getSample(channel, index1),
                buffer->getSample(channel, index2)
            );
        }
    }
}

number hann(number x) {
    x = (x > 1 ? 1 : (x < 0 ? 0 : x));
    return 0.5 * (1 - rnbo_cos(6.28318530717958647692 * x));
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

Index vectorsize() const {
    return this->vs;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    RNBO_UNUSED(numInputs);
    RNBO_UNUSED(inputs);
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    this->phasor_01_perform(this->phasor_01_freq, this->signals[0], n);
    this->dspexpr_01_perform(this->signals[0], this->dspexpr_01_in2, this->signals[1], n);
    this->mstosamps_tilde_01_perform(this->mstosamps_tilde_01_ms, this->signals[0], n);
    this->mstosamps_tilde_02_perform(this->mstosamps_tilde_02_ms, this->signals[2], n);
    this->ip_01_perform(this->signals[3], n);
    this->ip_02_perform(this->signals[4], n);

    this->adsr_01_perform(
        this->adsr_01_attack,
        this->adsr_01_decay,
        this->adsr_01_sustain,
        this->adsr_01_release,
        this->zeroBuffer,
        this->signals[5],
        n
    );

    this->dspexpr_04_perform(this->signals[3], this->signals[5], this->signals[6], n);

    this->granulator_01_perform(
        this->signals[0],
        this->signals[2],
        this->signals[6],
        this->granulator_01_bchan,
        this->signals[4],
        this->signals[1],
        this->signals[3],
        this->signals[7],
        n
    );

    this->signaladder_01_perform(this->signals[3], this->signals[7], this->signals[4], n);
    this->scopetilde_01_perform(this->signals[4], this->zeroBuffer, n);
    this->dspexpr_02_perform(this->signals[3], this->signals[5], this->signals[6], n);
    this->gaintilde_01_perform(this->signals[6], out1, n);
    this->dspexpr_03_perform(this->signals[7], this->signals[5], this->signals[6], n);
    this->gaintilde_02_perform(this->signals[6], out2, n);
    this->scopetilde_02_perform(this->signals[5], this->zeroBuffer, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 8; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->phasor_01_sigbuf = resizeSignal(this->phasor_01_sigbuf, this->maxvs, maxBlockSize);
        this->ip_01_sigbuf = resizeSignal(this->ip_01_sigbuf, this->maxvs, maxBlockSize);
        this->ip_02_sigbuf = resizeSignal(this->ip_02_sigbuf, this->maxvs, maxBlockSize);
        this->adsr_01_triggerBuf = resizeSignal(this->adsr_01_triggerBuf, this->maxvs, maxBlockSize);
        this->adsr_01_triggerValueBuf = resizeSignal(this->adsr_01_triggerValueBuf, this->maxvs, maxBlockSize);
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->phasor_01_dspsetup(forceDSPSetup);
    this->ip_01_dspsetup(forceDSPSetup);
    this->ip_02_dspsetup(forceDSPSetup);
    this->data_01_dspsetup(forceDSPSetup);
    this->adsr_01_dspsetup(forceDSPSetup);
    this->scopetilde_01_dspsetup(forceDSPSetup);
    this->gaintilde_01_dspsetup(forceDSPSetup);
    this->gaintilde_02_dspsetup(forceDSPSetup);
    this->scopetilde_02_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->drums);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 1;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->granulator_01_buffer = new Float32Buffer(this->drums);
        this->granulator_01_bufferUpdated();
        this->data_01_buffer = new Float32Buffer(this->drums);
        this->data_01_bufferUpdated();
    }
}

void initialize() {
    this->drums = initDataRef("drums", false, "Plaits_20200805_10", "buffer~");
    this->assign_defaults();
    this->setState();
    this->drums->setIndex(0);
    this->granulator_01_buffer = new Float32Buffer(this->drums);
    this->data_01_buffer = new Float32Buffer(this->drums);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double samplerate) {
    this->granulator_01_onSampleRateChanged(samplerate);
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "gain"));
    this->param_02_getPresetValue(getSubState(preset, "triggerSpeed"));
    this->param_03_getPresetValue(getSubState(preset, "startTime"));
    this->param_04_getPresetValue(getSubState(preset, "grLenght"));
    this->param_05_getPresetValue(getSubState(preset, "pbSpeed"));
    this->param_06_getPresetValue(getSubState(preset, "ui_XYgraph_trigger"));
    this->param_07_getPresetValue(getSubState(preset, "attack"));
    this->param_08_getPresetValue(getSubState(preset, "release"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "gain"));
    this->param_02_setPresetValue(getSubState(preset, "triggerSpeed"));
    this->param_03_setPresetValue(getSubState(preset, "startTime"));
    this->param_04_setPresetValue(getSubState(preset, "grLenght"));
    this->param_05_setPresetValue(getSubState(preset, "pbSpeed"));
    this->param_06_setPresetValue(getSubState(preset, "ui_XYgraph_trigger"));
    this->param_07_setPresetValue(getSubState(preset, "attack"));
    this->param_08_setPresetValue(getSubState(preset, "release"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    case 7:
        {
        this->param_08_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    case 7:
        {
        return this->param_08_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 8;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "gain";
        }
    case 1:
        {
        return "triggerSpeed";
        }
    case 2:
        {
        return "startTime";
        }
    case 3:
        {
        return "grLenght";
        }
    case 4:
        {
        return "pbSpeed";
        }
    case 5:
        {
        return "ui_XYgraph_trigger";
        }
    case 6:
        {
        return "attack";
        }
    case 7:
        {
        return "release";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "gain";
        }
    case 1:
        {
        return "triggerSpeed";
        }
    case 2:
        {
        return "startTime";
        }
    case 3:
        {
        return "grLenght";
        }
    case 4:
        {
        return "pbSpeed";
        }
    case 5:
        {
        return "ui_XYgraph_trigger";
        }
    case 6:
        {
        return "attack";
        }
    case 7:
        {
        return "release";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 124;
            info->min = 0;
            info->max = 128;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 6;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 3478;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 300;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = -20;
            info->max = 20;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 1;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 10;
            info->min = 0;
            info->max = 80000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 7:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 500;
            info->min = 0;
            info->max = 80000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);

            {
                normalizedValue = this->applyStepsToNormalizedParameterValue(normalizedValue, 1);
            }

            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 128 ? 128 : value));
            ParameterValue normalizedValue = (value - 0) / (128 - 0);
            return normalizedValue;
        }
        }
    case 1:
    case 3:
        {
        {
            value = (value < 0 ? 0 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 0) / (1000 - 0);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 3478 ? 3478 : value));
            ParameterValue normalizedValue = (value - 0) / (3478 - 0);
            return normalizedValue;
        }
        }
    case 6:
    case 7:
        {
        {
            value = (value < 0 ? 0 : (value > 80000 ? 80000 : value));
            ParameterValue normalizedValue = (value - 0) / (80000 - 0);
            return normalizedValue;
        }
        }
    case 4:
        {
        {
            value = (value < -20 ? -20 : (value > 20 ? 20 : value));
            ParameterValue normalizedValue = (value - -20) / (20 - -20);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 5:
        {
        {
            {
                value = this->applyStepsToNormalizedParameterValue(value, 1);
            }

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 0:
        {
        {
            {
                return 0 + value * (128 - 0);
            }
        }
        }
    case 1:
    case 3:
        {
        {
            {
                return 0 + value * (1000 - 0);
            }
        }
        }
    case 2:
        {
        {
            {
                return 0 + value * (3478 - 0);
            }
        }
        }
    case 6:
    case 7:
        {
        {
            {
                return 0 + value * (80000 - 0);
            }
        }
        }
    case 4:
        {
        {
            {
                return -20 + value * (20 - -20);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    case 7:
        {
        return this->param_08_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime time, ClockId index, bool hasValue, ParameterValue value) {
    RNBO_UNUSED(value);
    RNBO_UNUSED(hasValue);
    this->updateTime(time);

    switch (index) {
    case -1468824490:
        {
        this->adsr_01_mute_bang();
        break;
        }
    }
}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag tag, MessageTag objectId, MillisecondTime time, number payload) {
    this->updateTime(time);

    switch (tag) {
    case TAG("valin"):
        {
        if (TAG("gain~_obj-5") == objectId)
            this->gaintilde_01_valin_set(payload);

        if (TAG("gain~_obj-11") == objectId)
            this->gaintilde_02_valin_set(payload);

        if (TAG("number_obj-34") == objectId)
            this->numberobj_01_valin_set(payload);

        if (TAG("number_obj-33") == objectId)
            this->numberobj_02_valin_set(payload);

        break;
        }
    case TAG("format"):
        {
        if (TAG("number_obj-34") == objectId)
            this->numberobj_01_format_set(payload);

        if (TAG("number_obj-33") == objectId)
            this->numberobj_02_format_set(payload);

        break;
        }
    }
}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {
    case TAG("valout"):
        {
        return "valout";
        }
    case TAG("gain~_obj-5"):
        {
        return "gain~_obj-5";
        }
    case TAG("gain~_obj-11"):
        {
        return "gain~_obj-11";
        }
    case TAG("setup"):
        {
        return "setup";
        }
    case TAG("scope~_obj-7"):
        {
        return "scope~_obj-7";
        }
    case TAG("monitor"):
        {
        return "monitor";
        }
    case TAG("scope~_obj-10"):
        {
        return "scope~_obj-10";
        }
    case TAG("number_obj-34"):
        {
        return "number_obj-34";
        }
    case TAG("number_obj-33"):
        {
        return "number_obj-33";
        }
    case TAG("valin"):
        {
        return "valin";
        }
    case TAG("format"):
        {
        return "format";
        }
    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gaintilde_02_input_number_set(v);
    this->gaintilde_01_input_number_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->phasor_01_freq_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->mstosamps_tilde_01_ms_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->mstosamps_tilde_02_ms_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->ip_01_value_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->adsr_01_trigger_number_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->adsr_01_attack_set(v);
}

void param_08_value_set(number v) {
    v = this->param_08_value_constrain(v);
    this->param_08_value = v;
    this->sendParameter(7, false);

    if (this->param_08_value != this->param_08_lastValue) {
        this->getEngine()->presetTouched();
        this->param_08_lastValue = this->param_08_value;
    }

    this->adsr_01_release_set(v);
}

void gaintilde_01_valin_set(number v) {
    this->gaintilde_01_value_set(v);
}

void gaintilde_02_valin_set(number v) {
    this->gaintilde_02_value_set(v);
}

void numberobj_01_valin_set(number v) {
    this->numberobj_01_value_set(v);
}

void numberobj_01_format_set(number v) {
    this->numberobj_01_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_02_valin_set(number v) {
    this->numberobj_02_value_set(v);
}

void numberobj_02_format_set(number v) {
    this->numberobj_02_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void adsr_01_mute_bang() {}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 0;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->granulator_01_buffer = this->granulator_01_buffer->allocateIfNeeded();
    this->data_01_buffer = this->data_01_buffer->allocateIfNeeded();

    if (this->drums->hasRequestedSize()) {
        if (this->drums->wantsFill())
            this->zeroDataRef(this->drums);

        this->getEngine()->sendDataRefUpdated(0);
    }
}

void initializeObjects() {
    this->granulator_01_edgedetect_init();
    this->granulator_01_init();
    this->gaintilde_01_init();
    this->gaintilde_02_init();
    this->ip_01_init();
    this->ip_02_init();
    this->data_01_init();
    this->numberobj_01_init();
    this->numberobj_02_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    {
        this->scheduleParamInit(7, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 128 ? 128 : (v < 0 ? 0 : v));
    return v;
}

void gaintilde_02_outval_set(number ) {}

void gaintilde_02_value_set(number v) {
    this->gaintilde_02_value = v;
    number value;
    value = this->scale(v, 0, 157, 0, 158 - 1, 1);
    this->getEngine()->sendNumMessage(TAG("valout"), TAG("gain~_obj-11"), v, this->_currentTime);
    this->gaintilde_02_outval_set(value);
}

void gaintilde_02_input_number_set(number v) {
    this->gaintilde_02_input_number = v;

    this->gaintilde_02_value_set(
        this->scale((v > 158 - 1 ? 158 - 1 : (v < 0 ? 0 : v)), 0, 158 - 1, 0, 157, 1)
    );
}

void gaintilde_01_outval_set(number ) {}

void gaintilde_01_value_set(number v) {
    this->gaintilde_01_value = v;
    number value;
    value = this->scale(v, 0, 157, 0, 158 - 1, 1);
    this->getEngine()->sendNumMessage(TAG("valout"), TAG("gain~_obj-5"), v, this->_currentTime);
    this->gaintilde_01_outval_set(value);
}

void gaintilde_01_input_number_set(number v) {
    this->gaintilde_01_input_number = v;

    this->gaintilde_01_value_set(
        this->scale((v > 158 - 1 ? 158 - 1 : (v < 0 ? 0 : v)), 0, 158 - 1, 0, 157, 1)
    );
}

number param_02_value_constrain(number v) const {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void phasor_01_freq_set(number v) {
    this->phasor_01_freq = v;
}

number param_03_value_constrain(number v) const {
    v = (v > 3478 ? 3478 : (v < 0 ? 0 : v));
    return v;
}

void mstosamps_tilde_01_ms_set(number v) {
    this->mstosamps_tilde_01_ms = v;
}

number param_04_value_constrain(number v) const {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void mstosamps_tilde_02_ms_set(number v) {
    this->mstosamps_tilde_02_ms = v;
}

number param_05_value_constrain(number v) const {
    v = (v > 20 ? 20 : (v < -20 ? -20 : v));
    return v;
}

void ip_01_value_set(number v) {
    this->ip_01_value = v;
    this->ip_01_fillSigBuf();
    this->ip_01_lastValue = v;
}

number param_06_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));

    {
        number oneStep = 1 - 0;
        number oneStepInv = (oneStep != 0 ? (number)1 / oneStep : 0);
        number numberOfSteps = rnbo_fround(v * oneStepInv * 1 / (number)1) * 1;
        v = numberOfSteps * oneStep;
    }

    return v;
}

void adsr_01_trigger_number_set(number v) {
    this->adsr_01_trigger_number = v;

    if (v != 0)
        this->adsr_01_triggerBuf[(Index)this->sampleOffsetIntoNextAudioBuffer] = 1;

    for (number i = this->sampleOffsetIntoNextAudioBuffer; i < this->vectorsize(); i++) {
        this->adsr_01_triggerValueBuf[(Index)i] = v;
    }
}

number param_07_value_constrain(number v) const {
    v = (v > 80000 ? 80000 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_attack_set(number v) {
    this->adsr_01_attack = v;
}

number param_08_value_constrain(number v) const {
    v = (v > 80000 ? 80000 : (v < 0 ? 0 : v));
    return v;
}

void adsr_01_release_set(number v) {
    this->adsr_01_release = v;
}

void numberobj_01_output_set(number ) {}

void numberobj_01_value_set(number v) {
    this->numberobj_01_value_setter(v);
    v = this->numberobj_01_value;
    number localvalue = v;

    if (this->numberobj_01_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-34"), localvalue, this->_currentTime);
    this->numberobj_01_output_set(localvalue);
}

void numberobj_02_output_set(number ) {}

void numberobj_02_value_set(number v) {
    this->numberobj_02_value_setter(v);
    v = this->numberobj_02_value;
    number localvalue = v;

    if (this->numberobj_02_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-33"), localvalue, this->_currentTime);
    this->numberobj_02_output_set(localvalue);
}

void phasor_01_perform(number freq, SampleValue * out, Index n) {
    for (Index i = 0; i < n; i++) {
        out[(Index)i] = this->phasor_01_ph_next(freq, -1);
        this->phasor_01_sigbuf[(Index)i] = -1;
    }
}

void dspexpr_01_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] > 0.5;//#map:_###_obj_###_:1
    }
}

void mstosamps_tilde_01_perform(number ms, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = ms * this->sr * 0.001;
    }
}

void mstosamps_tilde_02_perform(number ms, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = ms * this->sr * 0.001;
    }
}

void ip_01_perform(SampleValue * out, Index n) {
    auto __ip_01_lastValue = this->ip_01_lastValue;
    auto __ip_01_lastIndex = this->ip_01_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_01_lastIndex ? __ip_01_lastValue : this->ip_01_sigbuf[(Index)i]);
    }

    __ip_01_lastIndex = 0;
    this->ip_01_lastIndex = __ip_01_lastIndex;
}

void ip_02_perform(SampleValue * out, Index n) {
    auto __ip_02_lastValue = this->ip_02_lastValue;
    auto __ip_02_lastIndex = this->ip_02_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_02_lastIndex ? __ip_02_lastValue : this->ip_02_sigbuf[(Index)i]);
    }

    __ip_02_lastIndex = 0;
    this->ip_02_lastIndex = __ip_02_lastIndex;
}

void adsr_01_perform(
    number attack,
    number decay,
    number sustain,
    number release,
    const SampleValue * trigger_signal,
    SampleValue * out,
    Index n
) {
    RNBO_UNUSED(trigger_signal);
    RNBO_UNUSED(sustain);
    RNBO_UNUSED(decay);
    auto __adsr_01_trigger_number = this->adsr_01_trigger_number;
    auto __adsr_01_time = this->adsr_01_time;
    auto __adsr_01_amplitude = this->adsr_01_amplitude;
    auto __adsr_01_outval = this->adsr_01_outval;
    auto __adsr_01_startingpoint = this->adsr_01_startingpoint;
    auto __adsr_01_phase = this->adsr_01_phase;
    auto __adsr_01_legato = this->adsr_01_legato;
    auto __adsr_01_lastTriggerVal = this->adsr_01_lastTriggerVal;
    auto __adsr_01_maxsustain = this->adsr_01_maxsustain;
    auto __adsr_01_mspersamp = this->adsr_01_mspersamp;
    bool bangMute = false;

    for (Index i = 0; i < n; i++) {
        number clampedattack = (attack > __adsr_01_mspersamp ? attack : __adsr_01_mspersamp);
        number clampeddecay = (0 > __adsr_01_mspersamp ? 0 : __adsr_01_mspersamp);
        number clampedsustain = (__adsr_01_maxsustain > __adsr_01_mspersamp ? __adsr_01_maxsustain : __adsr_01_mspersamp);
        number clampedrelease = (release > __adsr_01_mspersamp ? release : __adsr_01_mspersamp);
        number currentTriggerVal = this->adsr_01_triggerValueBuf[(Index)i];

        if ((__adsr_01_lastTriggerVal == 0.0 && currentTriggerVal != 0.0) || this->adsr_01_triggerBuf[(Index)i] == 1) {
            if ((bool)(__adsr_01_legato)) {
                if (__adsr_01_phase != 0) {
                    __adsr_01_startingpoint = __adsr_01_outval;
                } else {
                    __adsr_01_startingpoint = 0;
                }
            } else {
                __adsr_01_startingpoint = 0;
            }

            __adsr_01_amplitude = currentTriggerVal;
            __adsr_01_phase = 1;
            __adsr_01_time = 0.0;
            bangMute = false;
        } else if (__adsr_01_lastTriggerVal != 0.0 && currentTriggerVal == 0.0) {
            if (__adsr_01_phase != 4 && __adsr_01_phase != 0) {
                __adsr_01_phase = 4;
                __adsr_01_amplitude = __adsr_01_outval;
                __adsr_01_time = 0.0;
            }
        }

        __adsr_01_time += __adsr_01_mspersamp;

        if (__adsr_01_phase == 0) {
            __adsr_01_outval = 0;
        } else if (__adsr_01_phase == 1) {
            if (__adsr_01_time > clampedattack) {
                __adsr_01_time -= clampedattack;
                __adsr_01_phase = 2;
                __adsr_01_outval = __adsr_01_amplitude;
            } else {
                __adsr_01_outval = (__adsr_01_amplitude - __adsr_01_startingpoint) * __adsr_01_time / clampedattack + __adsr_01_startingpoint;
            }
        } else if (__adsr_01_phase == 2) {
            if (__adsr_01_time > clampeddecay) {
                __adsr_01_time -= clampeddecay;
                __adsr_01_phase = 3;
                __adsr_01_outval = __adsr_01_amplitude * 1;
            } else {
                __adsr_01_outval = __adsr_01_amplitude * 1 + (__adsr_01_amplitude - __adsr_01_amplitude * 1) * (1. - __adsr_01_time / clampeddecay);
            }
        } else if (__adsr_01_phase == 3) {
            if (__adsr_01_time > clampedsustain && __adsr_01_maxsustain > -1) {
                __adsr_01_time -= clampedsustain;
                __adsr_01_phase = 4;
                __adsr_01_amplitude = __adsr_01_outval;
            } else {
                __adsr_01_outval = __adsr_01_amplitude * 1;
            }
        } else if (__adsr_01_phase == 4) {
            if (__adsr_01_time > clampedrelease) {
                __adsr_01_time = 0;
                __adsr_01_phase = 0;
                __adsr_01_outval = 0;
                __adsr_01_amplitude = 0;
                bangMute = true;
            } else {
                __adsr_01_outval = __adsr_01_amplitude * (1.0 - __adsr_01_time / clampedrelease);
            }
        }

        out[(Index)i] = __adsr_01_outval;
        this->adsr_01_triggerBuf[(Index)i] = 0;
        this->adsr_01_triggerValueBuf[(Index)i] = __adsr_01_trigger_number;
        __adsr_01_lastTriggerVal = currentTriggerVal;
    }

    if ((bool)(bangMute)) {
        this->getEngine()->scheduleClockEventWithValue(
            this,
            -1468824490,
            this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
            0
        );;
    }

    this->adsr_01_lastTriggerVal = __adsr_01_lastTriggerVal;
    this->adsr_01_phase = __adsr_01_phase;
    this->adsr_01_startingpoint = __adsr_01_startingpoint;
    this->adsr_01_outval = __adsr_01_outval;
    this->adsr_01_amplitude = __adsr_01_amplitude;
    this->adsr_01_time = __adsr_01_time;
}

void dspexpr_04_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void granulator_01_perform(
    const Sample * start,
    const Sample * length,
    const Sample * rate,
    number bchan,
    const Sample * panpos,
    const SampleValue * trigger,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    RNBO_UNUSED(bchan);
    auto __granulator_01_buffer = this->granulator_01_buffer;
    auto __granulator_01_limitLast = this->granulator_01_limitLast;
    auto __granulator_01_limit = this->granulator_01_limit;
    number o = 0;
    Index i;

    for (i = 0; i < n; i++) {
        bool trig = (bool)(this->granulator_01_edgedetect_next(trigger[(Index)i]) > 0.0);
        int limit = (int)((__granulator_01_limit > 32 ? 32 : (__granulator_01_limit < 0 ? 0 : __granulator_01_limit)));
        __granulator_01_limitLast = limit;
        out1[(Index)i] = 0;
        out2[(Index)i] = 0;
        number s = start[(Index)i];
        number r = rate[(Index)i];
        number l = length[(Index)i];

        for (Index j = 0; j < 32; j++) {
            number thistrig = 0;

            if ((bool)(trig) && ((bool)(this->granulator_01_grains_inactive(j)) && j < limit)) {
                trig = false;
                thistrig = 1;
                this->granulator_01_grainPan[(Index)j][0] = this->granulator_01_panfunc_next(panpos[(Index)i], 0);
                this->granulator_01_grainPan[(Index)j][1] = this->granulator_01_panfunc_next(panpos[(Index)i], 1);
            }

            if (thistrig != 0 || (bool)(!(bool)(this->granulator_01_grains_inactive(j)))) {
                array<number, 3> ret = this->granulator_01_grains_next(j, __granulator_01_buffer, thistrig, s, l, r, 0);
                number v = ret[0] * this->hann(ret[1]);
                out1[(Index)i] += v * this->granulator_01_grainPan[(Index)j][0];
                out2[(Index)i] += v * this->granulator_01_grainPan[(Index)j][1];
            }
        }
    }

    this->granulator_01_limitLast = __granulator_01_limitLast;
}

void signaladder_01_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void scopetilde_01_perform(const SampleValue * x, const SampleValue * y, Index n) {
    auto __scopetilde_01_ysign = this->scopetilde_01_ysign;
    auto __scopetilde_01_ymonitorvalue = this->scopetilde_01_ymonitorvalue;
    auto __scopetilde_01_xsign = this->scopetilde_01_xsign;
    auto __scopetilde_01_xmonitorvalue = this->scopetilde_01_xmonitorvalue;
    auto __scopetilde_01_mode = this->scopetilde_01_mode;

    for (Index i = 0; i < n; i++) {
        number xval = x[(Index)i];
        number yval = y[(Index)i];

        if (__scopetilde_01_mode == 1) {
            number xabsval = rnbo_abs(xval);

            if (xabsval > __scopetilde_01_xmonitorvalue) {
                __scopetilde_01_xmonitorvalue = xabsval;
                __scopetilde_01_xsign = (xval < 0 ? -1 : 1);
            }

            number yabsval = rnbo_abs(yval);

            if (yabsval > __scopetilde_01_ymonitorvalue) {
                __scopetilde_01_ymonitorvalue = yabsval;
                __scopetilde_01_ysign = (yval < 0 ? -1 : 1);
            }
        } else {
            __scopetilde_01_xmonitorvalue = xval;
            __scopetilde_01_xsign = 1;
            __scopetilde_01_ymonitorvalue = yval;
            __scopetilde_01_ysign = 1;
        }

        this->scopetilde_01_effectiveCount--;

        if (this->scopetilde_01_effectiveCount <= 0) {
            this->scopetilde_01_updateEffectiveCount();
            this->scopetilde_01_monitorbuffer->push(__scopetilde_01_xmonitorvalue * __scopetilde_01_xsign);

            if (__scopetilde_01_mode == 1)
                __scopetilde_01_xmonitorvalue = 0;

            if (this->scopetilde_01_monitorbuffer->length >= 128 * (1 + 0)) {
                this->getEngine()->sendListMessage(
                    TAG("monitor"),
                    TAG("scope~_obj-7"),
                    this->scopetilde_01_monitorbuffer,
                    this->_currentTime
                );;

                this->scopetilde_01_monitorbuffer->length = 0;
            }
        }
    }

    this->scopetilde_01_xmonitorvalue = __scopetilde_01_xmonitorvalue;
    this->scopetilde_01_xsign = __scopetilde_01_xsign;
    this->scopetilde_01_ymonitorvalue = __scopetilde_01_ymonitorvalue;
    this->scopetilde_01_ysign = __scopetilde_01_ysign;
}

void dspexpr_02_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void gaintilde_01_perform(const SampleValue * input_signal, SampleValue * output, Index n) {
    auto __gaintilde_01_interp = this->gaintilde_01_interp;
    auto __gaintilde_01_loginc = this->gaintilde_01_loginc;
    auto __gaintilde_01_zval = this->gaintilde_01_zval;
    auto __gaintilde_01_value = this->gaintilde_01_value;
    number mult = (__gaintilde_01_value <= 0 ? 0. : __gaintilde_01_zval * rnbo_exp(__gaintilde_01_value * __gaintilde_01_loginc));
    auto iv = this->mstosamps(__gaintilde_01_interp);

    for (Index i = 0; i < n; i++) {
        output[(Index)i] = input_signal[(Index)i] * this->gaintilde_01_ramp_next(mult, iv, iv);
    }
}

void dspexpr_03_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void gaintilde_02_perform(const SampleValue * input_signal, SampleValue * output, Index n) {
    auto __gaintilde_02_interp = this->gaintilde_02_interp;
    auto __gaintilde_02_loginc = this->gaintilde_02_loginc;
    auto __gaintilde_02_zval = this->gaintilde_02_zval;
    auto __gaintilde_02_value = this->gaintilde_02_value;
    number mult = (__gaintilde_02_value <= 0 ? 0. : __gaintilde_02_zval * rnbo_exp(__gaintilde_02_value * __gaintilde_02_loginc));
    auto iv = this->mstosamps(__gaintilde_02_interp);

    for (Index i = 0; i < n; i++) {
        output[(Index)i] = input_signal[(Index)i] * this->gaintilde_02_ramp_next(mult, iv, iv);
    }
}

void scopetilde_02_perform(const SampleValue * x, const SampleValue * y, Index n) {
    auto __scopetilde_02_ysign = this->scopetilde_02_ysign;
    auto __scopetilde_02_ymonitorvalue = this->scopetilde_02_ymonitorvalue;
    auto __scopetilde_02_xsign = this->scopetilde_02_xsign;
    auto __scopetilde_02_xmonitorvalue = this->scopetilde_02_xmonitorvalue;
    auto __scopetilde_02_mode = this->scopetilde_02_mode;

    for (Index i = 0; i < n; i++) {
        number xval = x[(Index)i];
        number yval = y[(Index)i];

        if (__scopetilde_02_mode == 1) {
            number xabsval = rnbo_abs(xval);

            if (xabsval > __scopetilde_02_xmonitorvalue) {
                __scopetilde_02_xmonitorvalue = xabsval;
                __scopetilde_02_xsign = (xval < 0 ? -1 : 1);
            }

            number yabsval = rnbo_abs(yval);

            if (yabsval > __scopetilde_02_ymonitorvalue) {
                __scopetilde_02_ymonitorvalue = yabsval;
                __scopetilde_02_ysign = (yval < 0 ? -1 : 1);
            }
        } else {
            __scopetilde_02_xmonitorvalue = xval;
            __scopetilde_02_xsign = 1;
            __scopetilde_02_ymonitorvalue = yval;
            __scopetilde_02_ysign = 1;
        }

        this->scopetilde_02_effectiveCount--;

        if (this->scopetilde_02_effectiveCount <= 0) {
            this->scopetilde_02_updateEffectiveCount();
            this->scopetilde_02_monitorbuffer->push(__scopetilde_02_xmonitorvalue * __scopetilde_02_xsign);

            if (__scopetilde_02_mode == 1)
                __scopetilde_02_xmonitorvalue = 0;

            if (this->scopetilde_02_monitorbuffer->length >= 128 * (1 + 0)) {
                this->getEngine()->sendListMessage(
                    TAG("monitor"),
                    TAG("scope~_obj-10"),
                    this->scopetilde_02_monitorbuffer,
                    this->_currentTime
                );;

                this->scopetilde_02_monitorbuffer->length = 0;
            }
        }
    }

    this->scopetilde_02_xmonitorvalue = __scopetilde_02_xmonitorvalue;
    this->scopetilde_02_xsign = __scopetilde_02_xsign;
    this->scopetilde_02_ymonitorvalue = __scopetilde_02_ymonitorvalue;
    this->scopetilde_02_ysign = __scopetilde_02_ysign;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void data_01_srout_set(number ) {}

void data_01_chanout_set(number ) {}

void data_01_sizeout_set(number v) {
    this->data_01_sizeout = v;
}

void numberobj_01_value_setter(number v) {
    number localvalue = v;

    if (this->numberobj_01_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_01_value = localvalue;
}

void numberobj_02_value_setter(number v) {
    number localvalue = v;

    if (this->numberobj_02_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_02_value = localvalue;
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

number phasor_01_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->phasor_01_ph_conv;

    if (this->phasor_01_ph_currentPhase < 0.)
        this->phasor_01_ph_currentPhase = 1. + this->phasor_01_ph_currentPhase;

    if (this->phasor_01_ph_currentPhase > 1.)
        this->phasor_01_ph_currentPhase = this->phasor_01_ph_currentPhase - 1.;

    number tmp = this->phasor_01_ph_currentPhase;
    this->phasor_01_ph_currentPhase += pincr;
    return tmp;
}

void phasor_01_ph_reset() {
    this->phasor_01_ph_currentPhase = 0;
}

void phasor_01_ph_dspsetup() {
    this->phasor_01_ph_conv = (number)1 / this->sr;
}

void phasor_01_dspsetup(bool force) {
    if ((bool)(this->phasor_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_01_conv = (number)1 / this->samplerate();
    this->phasor_01_setupDone = true;
    this->phasor_01_ph_dspsetup();
}

number granulator_01_edgedetect_next(number val) {
    if ((0 == 0 && val <= 0) || (0 == 1 && val > 0)) {
        this->granulator_01_edgedetect_active = false;
    } else if ((bool)(!(bool)(this->granulator_01_edgedetect_active))) {
        this->granulator_01_edgedetect_active = true;
        return 1.0;
    }

    return 0.0;
}

void granulator_01_edgedetect_init() {}

void granulator_01_edgedetect_reset() {
    this->granulator_01_edgedetect_active = false;
}

number granulator_01_panfunc_next(number pos, int channel) {
    {
        {
            number nchan_1 = 2 - 1;

            {
                pos = pos * nchan_1;
            }

            {
                {
                    if (pos >= 0) {
                        pos = this->safemod(pos, 2);
                    } else {
                        pos = this->safemod(2 + this->safemod(pos, 2), 2);
                    }

                    if (channel == 0 && pos >= nchan_1)
                        pos = pos - 2;
                }
            }

            pos = pos - channel;

            if (pos > -1 && pos < 1) {
                {
                    {
                        return this->safesqrt(1.0 - rnbo_abs(pos));
                    }
                }
            } else {
                return 0;
            }
        }
    }
}

void granulator_01_panfunc_reset() {}

bool granulator_01_grains1_inactive() {
    return !(bool)(this->granulator_01_grains1_active);
}

void granulator_01_grains1_reset() {
    this->granulator_01_grains1_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains1_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains1_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains1_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains1_active)) {
                bangDone = true;
            }

            this->granulator_01_grains1_curRate = 0;
            this->granulator_01_grains1_active = false;
        } else {
            this->granulator_01_grains1_curLen = length;
            this->granulator_01_grains1_active = true;
            this->granulator_01_grains1_backwards = rate < 0;
            this->granulator_01_grains1_sampleOffset = 0;
            this->granulator_01_grains1_curStart = start;

            if ((bool)(this->granulator_01_grains1_backwards)) {
                this->granulator_01_grains1_curStart += this->granulator_01_grains1_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains1_active)) {
        if (this->granulator_01_grains1_sampleOffset >= this->granulator_01_grains1_curLen) {
            bangDone = true;
            this->granulator_01_grains1_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains1_curStart;

            if ((bool)(this->granulator_01_grains1_backwards)) {
                sampleIndex -= this->granulator_01_grains1_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains1_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains1_sampleOffset / this->granulator_01_grains1_curLen > 1 ? 1 : (this->granulator_01_grains1_sampleOffset / this->granulator_01_grains1_curLen < 0 ? 0 : this->granulator_01_grains1_sampleOffset / this->granulator_01_grains1_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains1_sampleOffset += this->granulator_01_grains1_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains2_inactive() {
    return !(bool)(this->granulator_01_grains2_active);
}

void granulator_01_grains2_reset() {
    this->granulator_01_grains2_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains2_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains2_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains2_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains2_active)) {
                bangDone = true;
            }

            this->granulator_01_grains2_curRate = 0;
            this->granulator_01_grains2_active = false;
        } else {
            this->granulator_01_grains2_curLen = length;
            this->granulator_01_grains2_active = true;
            this->granulator_01_grains2_backwards = rate < 0;
            this->granulator_01_grains2_sampleOffset = 0;
            this->granulator_01_grains2_curStart = start;

            if ((bool)(this->granulator_01_grains2_backwards)) {
                this->granulator_01_grains2_curStart += this->granulator_01_grains2_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains2_active)) {
        if (this->granulator_01_grains2_sampleOffset >= this->granulator_01_grains2_curLen) {
            bangDone = true;
            this->granulator_01_grains2_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains2_curStart;

            if ((bool)(this->granulator_01_grains2_backwards)) {
                sampleIndex -= this->granulator_01_grains2_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains2_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains2_sampleOffset / this->granulator_01_grains2_curLen > 1 ? 1 : (this->granulator_01_grains2_sampleOffset / this->granulator_01_grains2_curLen < 0 ? 0 : this->granulator_01_grains2_sampleOffset / this->granulator_01_grains2_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains2_sampleOffset += this->granulator_01_grains2_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains3_inactive() {
    return !(bool)(this->granulator_01_grains3_active);
}

void granulator_01_grains3_reset() {
    this->granulator_01_grains3_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains3_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains3_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains3_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains3_active)) {
                bangDone = true;
            }

            this->granulator_01_grains3_curRate = 0;
            this->granulator_01_grains3_active = false;
        } else {
            this->granulator_01_grains3_curLen = length;
            this->granulator_01_grains3_active = true;
            this->granulator_01_grains3_backwards = rate < 0;
            this->granulator_01_grains3_sampleOffset = 0;
            this->granulator_01_grains3_curStart = start;

            if ((bool)(this->granulator_01_grains3_backwards)) {
                this->granulator_01_grains3_curStart += this->granulator_01_grains3_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains3_active)) {
        if (this->granulator_01_grains3_sampleOffset >= this->granulator_01_grains3_curLen) {
            bangDone = true;
            this->granulator_01_grains3_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains3_curStart;

            if ((bool)(this->granulator_01_grains3_backwards)) {
                sampleIndex -= this->granulator_01_grains3_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains3_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains3_sampleOffset / this->granulator_01_grains3_curLen > 1 ? 1 : (this->granulator_01_grains3_sampleOffset / this->granulator_01_grains3_curLen < 0 ? 0 : this->granulator_01_grains3_sampleOffset / this->granulator_01_grains3_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains3_sampleOffset += this->granulator_01_grains3_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains4_inactive() {
    return !(bool)(this->granulator_01_grains4_active);
}

void granulator_01_grains4_reset() {
    this->granulator_01_grains4_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains4_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains4_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains4_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains4_active)) {
                bangDone = true;
            }

            this->granulator_01_grains4_curRate = 0;
            this->granulator_01_grains4_active = false;
        } else {
            this->granulator_01_grains4_curLen = length;
            this->granulator_01_grains4_active = true;
            this->granulator_01_grains4_backwards = rate < 0;
            this->granulator_01_grains4_sampleOffset = 0;
            this->granulator_01_grains4_curStart = start;

            if ((bool)(this->granulator_01_grains4_backwards)) {
                this->granulator_01_grains4_curStart += this->granulator_01_grains4_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains4_active)) {
        if (this->granulator_01_grains4_sampleOffset >= this->granulator_01_grains4_curLen) {
            bangDone = true;
            this->granulator_01_grains4_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains4_curStart;

            if ((bool)(this->granulator_01_grains4_backwards)) {
                sampleIndex -= this->granulator_01_grains4_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains4_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains4_sampleOffset / this->granulator_01_grains4_curLen > 1 ? 1 : (this->granulator_01_grains4_sampleOffset / this->granulator_01_grains4_curLen < 0 ? 0 : this->granulator_01_grains4_sampleOffset / this->granulator_01_grains4_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains4_sampleOffset += this->granulator_01_grains4_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains5_inactive() {
    return !(bool)(this->granulator_01_grains5_active);
}

void granulator_01_grains5_reset() {
    this->granulator_01_grains5_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains5_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains5_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains5_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains5_active)) {
                bangDone = true;
            }

            this->granulator_01_grains5_curRate = 0;
            this->granulator_01_grains5_active = false;
        } else {
            this->granulator_01_grains5_curLen = length;
            this->granulator_01_grains5_active = true;
            this->granulator_01_grains5_backwards = rate < 0;
            this->granulator_01_grains5_sampleOffset = 0;
            this->granulator_01_grains5_curStart = start;

            if ((bool)(this->granulator_01_grains5_backwards)) {
                this->granulator_01_grains5_curStart += this->granulator_01_grains5_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains5_active)) {
        if (this->granulator_01_grains5_sampleOffset >= this->granulator_01_grains5_curLen) {
            bangDone = true;
            this->granulator_01_grains5_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains5_curStart;

            if ((bool)(this->granulator_01_grains5_backwards)) {
                sampleIndex -= this->granulator_01_grains5_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains5_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains5_sampleOffset / this->granulator_01_grains5_curLen > 1 ? 1 : (this->granulator_01_grains5_sampleOffset / this->granulator_01_grains5_curLen < 0 ? 0 : this->granulator_01_grains5_sampleOffset / this->granulator_01_grains5_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains5_sampleOffset += this->granulator_01_grains5_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains6_inactive() {
    return !(bool)(this->granulator_01_grains6_active);
}

void granulator_01_grains6_reset() {
    this->granulator_01_grains6_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains6_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains6_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains6_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains6_active)) {
                bangDone = true;
            }

            this->granulator_01_grains6_curRate = 0;
            this->granulator_01_grains6_active = false;
        } else {
            this->granulator_01_grains6_curLen = length;
            this->granulator_01_grains6_active = true;
            this->granulator_01_grains6_backwards = rate < 0;
            this->granulator_01_grains6_sampleOffset = 0;
            this->granulator_01_grains6_curStart = start;

            if ((bool)(this->granulator_01_grains6_backwards)) {
                this->granulator_01_grains6_curStart += this->granulator_01_grains6_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains6_active)) {
        if (this->granulator_01_grains6_sampleOffset >= this->granulator_01_grains6_curLen) {
            bangDone = true;
            this->granulator_01_grains6_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains6_curStart;

            if ((bool)(this->granulator_01_grains6_backwards)) {
                sampleIndex -= this->granulator_01_grains6_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains6_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains6_sampleOffset / this->granulator_01_grains6_curLen > 1 ? 1 : (this->granulator_01_grains6_sampleOffset / this->granulator_01_grains6_curLen < 0 ? 0 : this->granulator_01_grains6_sampleOffset / this->granulator_01_grains6_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains6_sampleOffset += this->granulator_01_grains6_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains7_inactive() {
    return !(bool)(this->granulator_01_grains7_active);
}

void granulator_01_grains7_reset() {
    this->granulator_01_grains7_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains7_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains7_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains7_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains7_active)) {
                bangDone = true;
            }

            this->granulator_01_grains7_curRate = 0;
            this->granulator_01_grains7_active = false;
        } else {
            this->granulator_01_grains7_curLen = length;
            this->granulator_01_grains7_active = true;
            this->granulator_01_grains7_backwards = rate < 0;
            this->granulator_01_grains7_sampleOffset = 0;
            this->granulator_01_grains7_curStart = start;

            if ((bool)(this->granulator_01_grains7_backwards)) {
                this->granulator_01_grains7_curStart += this->granulator_01_grains7_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains7_active)) {
        if (this->granulator_01_grains7_sampleOffset >= this->granulator_01_grains7_curLen) {
            bangDone = true;
            this->granulator_01_grains7_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains7_curStart;

            if ((bool)(this->granulator_01_grains7_backwards)) {
                sampleIndex -= this->granulator_01_grains7_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains7_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains7_sampleOffset / this->granulator_01_grains7_curLen > 1 ? 1 : (this->granulator_01_grains7_sampleOffset / this->granulator_01_grains7_curLen < 0 ? 0 : this->granulator_01_grains7_sampleOffset / this->granulator_01_grains7_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains7_sampleOffset += this->granulator_01_grains7_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains8_inactive() {
    return !(bool)(this->granulator_01_grains8_active);
}

void granulator_01_grains8_reset() {
    this->granulator_01_grains8_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains8_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains8_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains8_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains8_active)) {
                bangDone = true;
            }

            this->granulator_01_grains8_curRate = 0;
            this->granulator_01_grains8_active = false;
        } else {
            this->granulator_01_grains8_curLen = length;
            this->granulator_01_grains8_active = true;
            this->granulator_01_grains8_backwards = rate < 0;
            this->granulator_01_grains8_sampleOffset = 0;
            this->granulator_01_grains8_curStart = start;

            if ((bool)(this->granulator_01_grains8_backwards)) {
                this->granulator_01_grains8_curStart += this->granulator_01_grains8_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains8_active)) {
        if (this->granulator_01_grains8_sampleOffset >= this->granulator_01_grains8_curLen) {
            bangDone = true;
            this->granulator_01_grains8_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains8_curStart;

            if ((bool)(this->granulator_01_grains8_backwards)) {
                sampleIndex -= this->granulator_01_grains8_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains8_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains8_sampleOffset / this->granulator_01_grains8_curLen > 1 ? 1 : (this->granulator_01_grains8_sampleOffset / this->granulator_01_grains8_curLen < 0 ? 0 : this->granulator_01_grains8_sampleOffset / this->granulator_01_grains8_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains8_sampleOffset += this->granulator_01_grains8_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains9_inactive() {
    return !(bool)(this->granulator_01_grains9_active);
}

void granulator_01_grains9_reset() {
    this->granulator_01_grains9_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains9_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains9_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains9_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains9_active)) {
                bangDone = true;
            }

            this->granulator_01_grains9_curRate = 0;
            this->granulator_01_grains9_active = false;
        } else {
            this->granulator_01_grains9_curLen = length;
            this->granulator_01_grains9_active = true;
            this->granulator_01_grains9_backwards = rate < 0;
            this->granulator_01_grains9_sampleOffset = 0;
            this->granulator_01_grains9_curStart = start;

            if ((bool)(this->granulator_01_grains9_backwards)) {
                this->granulator_01_grains9_curStart += this->granulator_01_grains9_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains9_active)) {
        if (this->granulator_01_grains9_sampleOffset >= this->granulator_01_grains9_curLen) {
            bangDone = true;
            this->granulator_01_grains9_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains9_curStart;

            if ((bool)(this->granulator_01_grains9_backwards)) {
                sampleIndex -= this->granulator_01_grains9_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains9_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains9_sampleOffset / this->granulator_01_grains9_curLen > 1 ? 1 : (this->granulator_01_grains9_sampleOffset / this->granulator_01_grains9_curLen < 0 ? 0 : this->granulator_01_grains9_sampleOffset / this->granulator_01_grains9_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains9_sampleOffset += this->granulator_01_grains9_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains10_inactive() {
    return !(bool)(this->granulator_01_grains10_active);
}

void granulator_01_grains10_reset() {
    this->granulator_01_grains10_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains10_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains10_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains10_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains10_active)) {
                bangDone = true;
            }

            this->granulator_01_grains10_curRate = 0;
            this->granulator_01_grains10_active = false;
        } else {
            this->granulator_01_grains10_curLen = length;
            this->granulator_01_grains10_active = true;
            this->granulator_01_grains10_backwards = rate < 0;
            this->granulator_01_grains10_sampleOffset = 0;
            this->granulator_01_grains10_curStart = start;

            if ((bool)(this->granulator_01_grains10_backwards)) {
                this->granulator_01_grains10_curStart += this->granulator_01_grains10_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains10_active)) {
        if (this->granulator_01_grains10_sampleOffset >= this->granulator_01_grains10_curLen) {
            bangDone = true;
            this->granulator_01_grains10_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains10_curStart;

            if ((bool)(this->granulator_01_grains10_backwards)) {
                sampleIndex -= this->granulator_01_grains10_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains10_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains10_sampleOffset / this->granulator_01_grains10_curLen > 1 ? 1 : (this->granulator_01_grains10_sampleOffset / this->granulator_01_grains10_curLen < 0 ? 0 : this->granulator_01_grains10_sampleOffset / this->granulator_01_grains10_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains10_sampleOffset += this->granulator_01_grains10_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains11_inactive() {
    return !(bool)(this->granulator_01_grains11_active);
}

void granulator_01_grains11_reset() {
    this->granulator_01_grains11_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains11_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains11_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains11_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains11_active)) {
                bangDone = true;
            }

            this->granulator_01_grains11_curRate = 0;
            this->granulator_01_grains11_active = false;
        } else {
            this->granulator_01_grains11_curLen = length;
            this->granulator_01_grains11_active = true;
            this->granulator_01_grains11_backwards = rate < 0;
            this->granulator_01_grains11_sampleOffset = 0;
            this->granulator_01_grains11_curStart = start;

            if ((bool)(this->granulator_01_grains11_backwards)) {
                this->granulator_01_grains11_curStart += this->granulator_01_grains11_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains11_active)) {
        if (this->granulator_01_grains11_sampleOffset >= this->granulator_01_grains11_curLen) {
            bangDone = true;
            this->granulator_01_grains11_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains11_curStart;

            if ((bool)(this->granulator_01_grains11_backwards)) {
                sampleIndex -= this->granulator_01_grains11_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains11_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains11_sampleOffset / this->granulator_01_grains11_curLen > 1 ? 1 : (this->granulator_01_grains11_sampleOffset / this->granulator_01_grains11_curLen < 0 ? 0 : this->granulator_01_grains11_sampleOffset / this->granulator_01_grains11_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains11_sampleOffset += this->granulator_01_grains11_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains12_inactive() {
    return !(bool)(this->granulator_01_grains12_active);
}

void granulator_01_grains12_reset() {
    this->granulator_01_grains12_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains12_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains12_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains12_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains12_active)) {
                bangDone = true;
            }

            this->granulator_01_grains12_curRate = 0;
            this->granulator_01_grains12_active = false;
        } else {
            this->granulator_01_grains12_curLen = length;
            this->granulator_01_grains12_active = true;
            this->granulator_01_grains12_backwards = rate < 0;
            this->granulator_01_grains12_sampleOffset = 0;
            this->granulator_01_grains12_curStart = start;

            if ((bool)(this->granulator_01_grains12_backwards)) {
                this->granulator_01_grains12_curStart += this->granulator_01_grains12_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains12_active)) {
        if (this->granulator_01_grains12_sampleOffset >= this->granulator_01_grains12_curLen) {
            bangDone = true;
            this->granulator_01_grains12_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains12_curStart;

            if ((bool)(this->granulator_01_grains12_backwards)) {
                sampleIndex -= this->granulator_01_grains12_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains12_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains12_sampleOffset / this->granulator_01_grains12_curLen > 1 ? 1 : (this->granulator_01_grains12_sampleOffset / this->granulator_01_grains12_curLen < 0 ? 0 : this->granulator_01_grains12_sampleOffset / this->granulator_01_grains12_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains12_sampleOffset += this->granulator_01_grains12_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains13_inactive() {
    return !(bool)(this->granulator_01_grains13_active);
}

void granulator_01_grains13_reset() {
    this->granulator_01_grains13_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains13_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains13_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains13_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains13_active)) {
                bangDone = true;
            }

            this->granulator_01_grains13_curRate = 0;
            this->granulator_01_grains13_active = false;
        } else {
            this->granulator_01_grains13_curLen = length;
            this->granulator_01_grains13_active = true;
            this->granulator_01_grains13_backwards = rate < 0;
            this->granulator_01_grains13_sampleOffset = 0;
            this->granulator_01_grains13_curStart = start;

            if ((bool)(this->granulator_01_grains13_backwards)) {
                this->granulator_01_grains13_curStart += this->granulator_01_grains13_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains13_active)) {
        if (this->granulator_01_grains13_sampleOffset >= this->granulator_01_grains13_curLen) {
            bangDone = true;
            this->granulator_01_grains13_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains13_curStart;

            if ((bool)(this->granulator_01_grains13_backwards)) {
                sampleIndex -= this->granulator_01_grains13_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains13_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains13_sampleOffset / this->granulator_01_grains13_curLen > 1 ? 1 : (this->granulator_01_grains13_sampleOffset / this->granulator_01_grains13_curLen < 0 ? 0 : this->granulator_01_grains13_sampleOffset / this->granulator_01_grains13_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains13_sampleOffset += this->granulator_01_grains13_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains14_inactive() {
    return !(bool)(this->granulator_01_grains14_active);
}

void granulator_01_grains14_reset() {
    this->granulator_01_grains14_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains14_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains14_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains14_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains14_active)) {
                bangDone = true;
            }

            this->granulator_01_grains14_curRate = 0;
            this->granulator_01_grains14_active = false;
        } else {
            this->granulator_01_grains14_curLen = length;
            this->granulator_01_grains14_active = true;
            this->granulator_01_grains14_backwards = rate < 0;
            this->granulator_01_grains14_sampleOffset = 0;
            this->granulator_01_grains14_curStart = start;

            if ((bool)(this->granulator_01_grains14_backwards)) {
                this->granulator_01_grains14_curStart += this->granulator_01_grains14_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains14_active)) {
        if (this->granulator_01_grains14_sampleOffset >= this->granulator_01_grains14_curLen) {
            bangDone = true;
            this->granulator_01_grains14_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains14_curStart;

            if ((bool)(this->granulator_01_grains14_backwards)) {
                sampleIndex -= this->granulator_01_grains14_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains14_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains14_sampleOffset / this->granulator_01_grains14_curLen > 1 ? 1 : (this->granulator_01_grains14_sampleOffset / this->granulator_01_grains14_curLen < 0 ? 0 : this->granulator_01_grains14_sampleOffset / this->granulator_01_grains14_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains14_sampleOffset += this->granulator_01_grains14_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains15_inactive() {
    return !(bool)(this->granulator_01_grains15_active);
}

void granulator_01_grains15_reset() {
    this->granulator_01_grains15_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains15_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains15_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains15_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains15_active)) {
                bangDone = true;
            }

            this->granulator_01_grains15_curRate = 0;
            this->granulator_01_grains15_active = false;
        } else {
            this->granulator_01_grains15_curLen = length;
            this->granulator_01_grains15_active = true;
            this->granulator_01_grains15_backwards = rate < 0;
            this->granulator_01_grains15_sampleOffset = 0;
            this->granulator_01_grains15_curStart = start;

            if ((bool)(this->granulator_01_grains15_backwards)) {
                this->granulator_01_grains15_curStart += this->granulator_01_grains15_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains15_active)) {
        if (this->granulator_01_grains15_sampleOffset >= this->granulator_01_grains15_curLen) {
            bangDone = true;
            this->granulator_01_grains15_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains15_curStart;

            if ((bool)(this->granulator_01_grains15_backwards)) {
                sampleIndex -= this->granulator_01_grains15_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains15_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains15_sampleOffset / this->granulator_01_grains15_curLen > 1 ? 1 : (this->granulator_01_grains15_sampleOffset / this->granulator_01_grains15_curLen < 0 ? 0 : this->granulator_01_grains15_sampleOffset / this->granulator_01_grains15_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains15_sampleOffset += this->granulator_01_grains15_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains16_inactive() {
    return !(bool)(this->granulator_01_grains16_active);
}

void granulator_01_grains16_reset() {
    this->granulator_01_grains16_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains16_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains16_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains16_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains16_active)) {
                bangDone = true;
            }

            this->granulator_01_grains16_curRate = 0;
            this->granulator_01_grains16_active = false;
        } else {
            this->granulator_01_grains16_curLen = length;
            this->granulator_01_grains16_active = true;
            this->granulator_01_grains16_backwards = rate < 0;
            this->granulator_01_grains16_sampleOffset = 0;
            this->granulator_01_grains16_curStart = start;

            if ((bool)(this->granulator_01_grains16_backwards)) {
                this->granulator_01_grains16_curStart += this->granulator_01_grains16_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains16_active)) {
        if (this->granulator_01_grains16_sampleOffset >= this->granulator_01_grains16_curLen) {
            bangDone = true;
            this->granulator_01_grains16_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains16_curStart;

            if ((bool)(this->granulator_01_grains16_backwards)) {
                sampleIndex -= this->granulator_01_grains16_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains16_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains16_sampleOffset / this->granulator_01_grains16_curLen > 1 ? 1 : (this->granulator_01_grains16_sampleOffset / this->granulator_01_grains16_curLen < 0 ? 0 : this->granulator_01_grains16_sampleOffset / this->granulator_01_grains16_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains16_sampleOffset += this->granulator_01_grains16_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains17_inactive() {
    return !(bool)(this->granulator_01_grains17_active);
}

void granulator_01_grains17_reset() {
    this->granulator_01_grains17_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains17_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains17_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains17_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains17_active)) {
                bangDone = true;
            }

            this->granulator_01_grains17_curRate = 0;
            this->granulator_01_grains17_active = false;
        } else {
            this->granulator_01_grains17_curLen = length;
            this->granulator_01_grains17_active = true;
            this->granulator_01_grains17_backwards = rate < 0;
            this->granulator_01_grains17_sampleOffset = 0;
            this->granulator_01_grains17_curStart = start;

            if ((bool)(this->granulator_01_grains17_backwards)) {
                this->granulator_01_grains17_curStart += this->granulator_01_grains17_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains17_active)) {
        if (this->granulator_01_grains17_sampleOffset >= this->granulator_01_grains17_curLen) {
            bangDone = true;
            this->granulator_01_grains17_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains17_curStart;

            if ((bool)(this->granulator_01_grains17_backwards)) {
                sampleIndex -= this->granulator_01_grains17_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains17_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains17_sampleOffset / this->granulator_01_grains17_curLen > 1 ? 1 : (this->granulator_01_grains17_sampleOffset / this->granulator_01_grains17_curLen < 0 ? 0 : this->granulator_01_grains17_sampleOffset / this->granulator_01_grains17_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains17_sampleOffset += this->granulator_01_grains17_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains18_inactive() {
    return !(bool)(this->granulator_01_grains18_active);
}

void granulator_01_grains18_reset() {
    this->granulator_01_grains18_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains18_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains18_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains18_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains18_active)) {
                bangDone = true;
            }

            this->granulator_01_grains18_curRate = 0;
            this->granulator_01_grains18_active = false;
        } else {
            this->granulator_01_grains18_curLen = length;
            this->granulator_01_grains18_active = true;
            this->granulator_01_grains18_backwards = rate < 0;
            this->granulator_01_grains18_sampleOffset = 0;
            this->granulator_01_grains18_curStart = start;

            if ((bool)(this->granulator_01_grains18_backwards)) {
                this->granulator_01_grains18_curStart += this->granulator_01_grains18_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains18_active)) {
        if (this->granulator_01_grains18_sampleOffset >= this->granulator_01_grains18_curLen) {
            bangDone = true;
            this->granulator_01_grains18_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains18_curStart;

            if ((bool)(this->granulator_01_grains18_backwards)) {
                sampleIndex -= this->granulator_01_grains18_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains18_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains18_sampleOffset / this->granulator_01_grains18_curLen > 1 ? 1 : (this->granulator_01_grains18_sampleOffset / this->granulator_01_grains18_curLen < 0 ? 0 : this->granulator_01_grains18_sampleOffset / this->granulator_01_grains18_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains18_sampleOffset += this->granulator_01_grains18_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains19_inactive() {
    return !(bool)(this->granulator_01_grains19_active);
}

void granulator_01_grains19_reset() {
    this->granulator_01_grains19_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains19_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains19_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains19_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains19_active)) {
                bangDone = true;
            }

            this->granulator_01_grains19_curRate = 0;
            this->granulator_01_grains19_active = false;
        } else {
            this->granulator_01_grains19_curLen = length;
            this->granulator_01_grains19_active = true;
            this->granulator_01_grains19_backwards = rate < 0;
            this->granulator_01_grains19_sampleOffset = 0;
            this->granulator_01_grains19_curStart = start;

            if ((bool)(this->granulator_01_grains19_backwards)) {
                this->granulator_01_grains19_curStart += this->granulator_01_grains19_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains19_active)) {
        if (this->granulator_01_grains19_sampleOffset >= this->granulator_01_grains19_curLen) {
            bangDone = true;
            this->granulator_01_grains19_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains19_curStart;

            if ((bool)(this->granulator_01_grains19_backwards)) {
                sampleIndex -= this->granulator_01_grains19_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains19_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains19_sampleOffset / this->granulator_01_grains19_curLen > 1 ? 1 : (this->granulator_01_grains19_sampleOffset / this->granulator_01_grains19_curLen < 0 ? 0 : this->granulator_01_grains19_sampleOffset / this->granulator_01_grains19_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains19_sampleOffset += this->granulator_01_grains19_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains20_inactive() {
    return !(bool)(this->granulator_01_grains20_active);
}

void granulator_01_grains20_reset() {
    this->granulator_01_grains20_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains20_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains20_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains20_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains20_active)) {
                bangDone = true;
            }

            this->granulator_01_grains20_curRate = 0;
            this->granulator_01_grains20_active = false;
        } else {
            this->granulator_01_grains20_curLen = length;
            this->granulator_01_grains20_active = true;
            this->granulator_01_grains20_backwards = rate < 0;
            this->granulator_01_grains20_sampleOffset = 0;
            this->granulator_01_grains20_curStart = start;

            if ((bool)(this->granulator_01_grains20_backwards)) {
                this->granulator_01_grains20_curStart += this->granulator_01_grains20_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains20_active)) {
        if (this->granulator_01_grains20_sampleOffset >= this->granulator_01_grains20_curLen) {
            bangDone = true;
            this->granulator_01_grains20_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains20_curStart;

            if ((bool)(this->granulator_01_grains20_backwards)) {
                sampleIndex -= this->granulator_01_grains20_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains20_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains20_sampleOffset / this->granulator_01_grains20_curLen > 1 ? 1 : (this->granulator_01_grains20_sampleOffset / this->granulator_01_grains20_curLen < 0 ? 0 : this->granulator_01_grains20_sampleOffset / this->granulator_01_grains20_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains20_sampleOffset += this->granulator_01_grains20_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains21_inactive() {
    return !(bool)(this->granulator_01_grains21_active);
}

void granulator_01_grains21_reset() {
    this->granulator_01_grains21_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains21_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains21_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains21_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains21_active)) {
                bangDone = true;
            }

            this->granulator_01_grains21_curRate = 0;
            this->granulator_01_grains21_active = false;
        } else {
            this->granulator_01_grains21_curLen = length;
            this->granulator_01_grains21_active = true;
            this->granulator_01_grains21_backwards = rate < 0;
            this->granulator_01_grains21_sampleOffset = 0;
            this->granulator_01_grains21_curStart = start;

            if ((bool)(this->granulator_01_grains21_backwards)) {
                this->granulator_01_grains21_curStart += this->granulator_01_grains21_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains21_active)) {
        if (this->granulator_01_grains21_sampleOffset >= this->granulator_01_grains21_curLen) {
            bangDone = true;
            this->granulator_01_grains21_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains21_curStart;

            if ((bool)(this->granulator_01_grains21_backwards)) {
                sampleIndex -= this->granulator_01_grains21_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains21_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains21_sampleOffset / this->granulator_01_grains21_curLen > 1 ? 1 : (this->granulator_01_grains21_sampleOffset / this->granulator_01_grains21_curLen < 0 ? 0 : this->granulator_01_grains21_sampleOffset / this->granulator_01_grains21_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains21_sampleOffset += this->granulator_01_grains21_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains22_inactive() {
    return !(bool)(this->granulator_01_grains22_active);
}

void granulator_01_grains22_reset() {
    this->granulator_01_grains22_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains22_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains22_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains22_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains22_active)) {
                bangDone = true;
            }

            this->granulator_01_grains22_curRate = 0;
            this->granulator_01_grains22_active = false;
        } else {
            this->granulator_01_grains22_curLen = length;
            this->granulator_01_grains22_active = true;
            this->granulator_01_grains22_backwards = rate < 0;
            this->granulator_01_grains22_sampleOffset = 0;
            this->granulator_01_grains22_curStart = start;

            if ((bool)(this->granulator_01_grains22_backwards)) {
                this->granulator_01_grains22_curStart += this->granulator_01_grains22_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains22_active)) {
        if (this->granulator_01_grains22_sampleOffset >= this->granulator_01_grains22_curLen) {
            bangDone = true;
            this->granulator_01_grains22_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains22_curStart;

            if ((bool)(this->granulator_01_grains22_backwards)) {
                sampleIndex -= this->granulator_01_grains22_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains22_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains22_sampleOffset / this->granulator_01_grains22_curLen > 1 ? 1 : (this->granulator_01_grains22_sampleOffset / this->granulator_01_grains22_curLen < 0 ? 0 : this->granulator_01_grains22_sampleOffset / this->granulator_01_grains22_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains22_sampleOffset += this->granulator_01_grains22_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains23_inactive() {
    return !(bool)(this->granulator_01_grains23_active);
}

void granulator_01_grains23_reset() {
    this->granulator_01_grains23_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains23_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains23_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains23_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains23_active)) {
                bangDone = true;
            }

            this->granulator_01_grains23_curRate = 0;
            this->granulator_01_grains23_active = false;
        } else {
            this->granulator_01_grains23_curLen = length;
            this->granulator_01_grains23_active = true;
            this->granulator_01_grains23_backwards = rate < 0;
            this->granulator_01_grains23_sampleOffset = 0;
            this->granulator_01_grains23_curStart = start;

            if ((bool)(this->granulator_01_grains23_backwards)) {
                this->granulator_01_grains23_curStart += this->granulator_01_grains23_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains23_active)) {
        if (this->granulator_01_grains23_sampleOffset >= this->granulator_01_grains23_curLen) {
            bangDone = true;
            this->granulator_01_grains23_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains23_curStart;

            if ((bool)(this->granulator_01_grains23_backwards)) {
                sampleIndex -= this->granulator_01_grains23_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains23_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains23_sampleOffset / this->granulator_01_grains23_curLen > 1 ? 1 : (this->granulator_01_grains23_sampleOffset / this->granulator_01_grains23_curLen < 0 ? 0 : this->granulator_01_grains23_sampleOffset / this->granulator_01_grains23_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains23_sampleOffset += this->granulator_01_grains23_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains24_inactive() {
    return !(bool)(this->granulator_01_grains24_active);
}

void granulator_01_grains24_reset() {
    this->granulator_01_grains24_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains24_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains24_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains24_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains24_active)) {
                bangDone = true;
            }

            this->granulator_01_grains24_curRate = 0;
            this->granulator_01_grains24_active = false;
        } else {
            this->granulator_01_grains24_curLen = length;
            this->granulator_01_grains24_active = true;
            this->granulator_01_grains24_backwards = rate < 0;
            this->granulator_01_grains24_sampleOffset = 0;
            this->granulator_01_grains24_curStart = start;

            if ((bool)(this->granulator_01_grains24_backwards)) {
                this->granulator_01_grains24_curStart += this->granulator_01_grains24_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains24_active)) {
        if (this->granulator_01_grains24_sampleOffset >= this->granulator_01_grains24_curLen) {
            bangDone = true;
            this->granulator_01_grains24_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains24_curStart;

            if ((bool)(this->granulator_01_grains24_backwards)) {
                sampleIndex -= this->granulator_01_grains24_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains24_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains24_sampleOffset / this->granulator_01_grains24_curLen > 1 ? 1 : (this->granulator_01_grains24_sampleOffset / this->granulator_01_grains24_curLen < 0 ? 0 : this->granulator_01_grains24_sampleOffset / this->granulator_01_grains24_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains24_sampleOffset += this->granulator_01_grains24_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains25_inactive() {
    return !(bool)(this->granulator_01_grains25_active);
}

void granulator_01_grains25_reset() {
    this->granulator_01_grains25_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains25_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains25_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains25_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains25_active)) {
                bangDone = true;
            }

            this->granulator_01_grains25_curRate = 0;
            this->granulator_01_grains25_active = false;
        } else {
            this->granulator_01_grains25_curLen = length;
            this->granulator_01_grains25_active = true;
            this->granulator_01_grains25_backwards = rate < 0;
            this->granulator_01_grains25_sampleOffset = 0;
            this->granulator_01_grains25_curStart = start;

            if ((bool)(this->granulator_01_grains25_backwards)) {
                this->granulator_01_grains25_curStart += this->granulator_01_grains25_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains25_active)) {
        if (this->granulator_01_grains25_sampleOffset >= this->granulator_01_grains25_curLen) {
            bangDone = true;
            this->granulator_01_grains25_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains25_curStart;

            if ((bool)(this->granulator_01_grains25_backwards)) {
                sampleIndex -= this->granulator_01_grains25_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains25_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains25_sampleOffset / this->granulator_01_grains25_curLen > 1 ? 1 : (this->granulator_01_grains25_sampleOffset / this->granulator_01_grains25_curLen < 0 ? 0 : this->granulator_01_grains25_sampleOffset / this->granulator_01_grains25_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains25_sampleOffset += this->granulator_01_grains25_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains26_inactive() {
    return !(bool)(this->granulator_01_grains26_active);
}

void granulator_01_grains26_reset() {
    this->granulator_01_grains26_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains26_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains26_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains26_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains26_active)) {
                bangDone = true;
            }

            this->granulator_01_grains26_curRate = 0;
            this->granulator_01_grains26_active = false;
        } else {
            this->granulator_01_grains26_curLen = length;
            this->granulator_01_grains26_active = true;
            this->granulator_01_grains26_backwards = rate < 0;
            this->granulator_01_grains26_sampleOffset = 0;
            this->granulator_01_grains26_curStart = start;

            if ((bool)(this->granulator_01_grains26_backwards)) {
                this->granulator_01_grains26_curStart += this->granulator_01_grains26_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains26_active)) {
        if (this->granulator_01_grains26_sampleOffset >= this->granulator_01_grains26_curLen) {
            bangDone = true;
            this->granulator_01_grains26_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains26_curStart;

            if ((bool)(this->granulator_01_grains26_backwards)) {
                sampleIndex -= this->granulator_01_grains26_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains26_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains26_sampleOffset / this->granulator_01_grains26_curLen > 1 ? 1 : (this->granulator_01_grains26_sampleOffset / this->granulator_01_grains26_curLen < 0 ? 0 : this->granulator_01_grains26_sampleOffset / this->granulator_01_grains26_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains26_sampleOffset += this->granulator_01_grains26_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains27_inactive() {
    return !(bool)(this->granulator_01_grains27_active);
}

void granulator_01_grains27_reset() {
    this->granulator_01_grains27_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains27_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains27_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains27_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains27_active)) {
                bangDone = true;
            }

            this->granulator_01_grains27_curRate = 0;
            this->granulator_01_grains27_active = false;
        } else {
            this->granulator_01_grains27_curLen = length;
            this->granulator_01_grains27_active = true;
            this->granulator_01_grains27_backwards = rate < 0;
            this->granulator_01_grains27_sampleOffset = 0;
            this->granulator_01_grains27_curStart = start;

            if ((bool)(this->granulator_01_grains27_backwards)) {
                this->granulator_01_grains27_curStart += this->granulator_01_grains27_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains27_active)) {
        if (this->granulator_01_grains27_sampleOffset >= this->granulator_01_grains27_curLen) {
            bangDone = true;
            this->granulator_01_grains27_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains27_curStart;

            if ((bool)(this->granulator_01_grains27_backwards)) {
                sampleIndex -= this->granulator_01_grains27_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains27_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains27_sampleOffset / this->granulator_01_grains27_curLen > 1 ? 1 : (this->granulator_01_grains27_sampleOffset / this->granulator_01_grains27_curLen < 0 ? 0 : this->granulator_01_grains27_sampleOffset / this->granulator_01_grains27_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains27_sampleOffset += this->granulator_01_grains27_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains28_inactive() {
    return !(bool)(this->granulator_01_grains28_active);
}

void granulator_01_grains28_reset() {
    this->granulator_01_grains28_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains28_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains28_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains28_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains28_active)) {
                bangDone = true;
            }

            this->granulator_01_grains28_curRate = 0;
            this->granulator_01_grains28_active = false;
        } else {
            this->granulator_01_grains28_curLen = length;
            this->granulator_01_grains28_active = true;
            this->granulator_01_grains28_backwards = rate < 0;
            this->granulator_01_grains28_sampleOffset = 0;
            this->granulator_01_grains28_curStart = start;

            if ((bool)(this->granulator_01_grains28_backwards)) {
                this->granulator_01_grains28_curStart += this->granulator_01_grains28_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains28_active)) {
        if (this->granulator_01_grains28_sampleOffset >= this->granulator_01_grains28_curLen) {
            bangDone = true;
            this->granulator_01_grains28_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains28_curStart;

            if ((bool)(this->granulator_01_grains28_backwards)) {
                sampleIndex -= this->granulator_01_grains28_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains28_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains28_sampleOffset / this->granulator_01_grains28_curLen > 1 ? 1 : (this->granulator_01_grains28_sampleOffset / this->granulator_01_grains28_curLen < 0 ? 0 : this->granulator_01_grains28_sampleOffset / this->granulator_01_grains28_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains28_sampleOffset += this->granulator_01_grains28_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains29_inactive() {
    return !(bool)(this->granulator_01_grains29_active);
}

void granulator_01_grains29_reset() {
    this->granulator_01_grains29_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains29_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains29_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains29_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains29_active)) {
                bangDone = true;
            }

            this->granulator_01_grains29_curRate = 0;
            this->granulator_01_grains29_active = false;
        } else {
            this->granulator_01_grains29_curLen = length;
            this->granulator_01_grains29_active = true;
            this->granulator_01_grains29_backwards = rate < 0;
            this->granulator_01_grains29_sampleOffset = 0;
            this->granulator_01_grains29_curStart = start;

            if ((bool)(this->granulator_01_grains29_backwards)) {
                this->granulator_01_grains29_curStart += this->granulator_01_grains29_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains29_active)) {
        if (this->granulator_01_grains29_sampleOffset >= this->granulator_01_grains29_curLen) {
            bangDone = true;
            this->granulator_01_grains29_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains29_curStart;

            if ((bool)(this->granulator_01_grains29_backwards)) {
                sampleIndex -= this->granulator_01_grains29_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains29_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains29_sampleOffset / this->granulator_01_grains29_curLen > 1 ? 1 : (this->granulator_01_grains29_sampleOffset / this->granulator_01_grains29_curLen < 0 ? 0 : this->granulator_01_grains29_sampleOffset / this->granulator_01_grains29_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains29_sampleOffset += this->granulator_01_grains29_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains30_inactive() {
    return !(bool)(this->granulator_01_grains30_active);
}

void granulator_01_grains30_reset() {
    this->granulator_01_grains30_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains30_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains30_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains30_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains30_active)) {
                bangDone = true;
            }

            this->granulator_01_grains30_curRate = 0;
            this->granulator_01_grains30_active = false;
        } else {
            this->granulator_01_grains30_curLen = length;
            this->granulator_01_grains30_active = true;
            this->granulator_01_grains30_backwards = rate < 0;
            this->granulator_01_grains30_sampleOffset = 0;
            this->granulator_01_grains30_curStart = start;

            if ((bool)(this->granulator_01_grains30_backwards)) {
                this->granulator_01_grains30_curStart += this->granulator_01_grains30_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains30_active)) {
        if (this->granulator_01_grains30_sampleOffset >= this->granulator_01_grains30_curLen) {
            bangDone = true;
            this->granulator_01_grains30_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains30_curStart;

            if ((bool)(this->granulator_01_grains30_backwards)) {
                sampleIndex -= this->granulator_01_grains30_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains30_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains30_sampleOffset / this->granulator_01_grains30_curLen > 1 ? 1 : (this->granulator_01_grains30_sampleOffset / this->granulator_01_grains30_curLen < 0 ? 0 : this->granulator_01_grains30_sampleOffset / this->granulator_01_grains30_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains30_sampleOffset += this->granulator_01_grains30_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains31_inactive() {
    return !(bool)(this->granulator_01_grains31_active);
}

void granulator_01_grains31_reset() {
    this->granulator_01_grains31_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains31_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains31_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains31_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains31_active)) {
                bangDone = true;
            }

            this->granulator_01_grains31_curRate = 0;
            this->granulator_01_grains31_active = false;
        } else {
            this->granulator_01_grains31_curLen = length;
            this->granulator_01_grains31_active = true;
            this->granulator_01_grains31_backwards = rate < 0;
            this->granulator_01_grains31_sampleOffset = 0;
            this->granulator_01_grains31_curStart = start;

            if ((bool)(this->granulator_01_grains31_backwards)) {
                this->granulator_01_grains31_curStart += this->granulator_01_grains31_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains31_active)) {
        if (this->granulator_01_grains31_sampleOffset >= this->granulator_01_grains31_curLen) {
            bangDone = true;
            this->granulator_01_grains31_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains31_curStart;

            if ((bool)(this->granulator_01_grains31_backwards)) {
                sampleIndex -= this->granulator_01_grains31_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains31_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains31_sampleOffset / this->granulator_01_grains31_curLen > 1 ? 1 : (this->granulator_01_grains31_sampleOffset / this->granulator_01_grains31_curLen < 0 ? 0 : this->granulator_01_grains31_sampleOffset / this->granulator_01_grains31_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains31_sampleOffset += this->granulator_01_grains31_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains32_inactive() {
    return !(bool)(this->granulator_01_grains32_active);
}

void granulator_01_grains32_reset() {
    this->granulator_01_grains32_active = false;
}

template <typename T> inline array<number, 3> granulator_01_grains32_next(
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    number bangDone = false;
    number v = 0;
    number ph = 0;
    const SampleIndex bufferSize = (const SampleIndex)(buffer->getSize());

    if (trigger > 0) {
        this->granulator_01_grains32_curRate = rnbo_abs(rate * buffer->getSampleRate() / this->samplerate());

        if (this->granulator_01_grains32_curRate == 0 || length <= 0) {
            if ((bool)(this->granulator_01_grains32_active)) {
                bangDone = true;
            }

            this->granulator_01_grains32_curRate = 0;
            this->granulator_01_grains32_active = false;
        } else {
            this->granulator_01_grains32_curLen = length;
            this->granulator_01_grains32_active = true;
            this->granulator_01_grains32_backwards = rate < 0;
            this->granulator_01_grains32_sampleOffset = 0;
            this->granulator_01_grains32_curStart = start;

            if ((bool)(this->granulator_01_grains32_backwards)) {
                this->granulator_01_grains32_curStart += this->granulator_01_grains32_curLen;
            }

            bangDone = false;
        }
    }

    if ((bool)(this->granulator_01_grains32_active)) {
        if (this->granulator_01_grains32_sampleOffset >= this->granulator_01_grains32_curLen) {
            bangDone = true;
            this->granulator_01_grains32_active = false;
            ph = 1.0;
        } else {
            number sampleIndex = this->granulator_01_grains32_curStart;

            if ((bool)(this->granulator_01_grains32_backwards)) {
                sampleIndex -= this->granulator_01_grains32_sampleOffset;
            } else {
                sampleIndex += this->granulator_01_grains32_sampleOffset;
            }

            const Index bufferChannels = (const Index)(buffer->getChannels());
            ph = (this->granulator_01_grains32_sampleOffset / this->granulator_01_grains32_curLen > 1 ? 1 : (this->granulator_01_grains32_sampleOffset / this->granulator_01_grains32_curLen < 0 ? 0 : this->granulator_01_grains32_sampleOffset / this->granulator_01_grains32_curLen));

            if (bufferSize == 0)
                v = 0;
            else {
                {
                    channel = this->bufferbindchannel(channel, bufferChannels, 1);
                    sampleIndex = this->bufferbindindex(sampleIndex, 0, bufferSize, 3);
                    v = this->bufferreadsample(buffer, sampleIndex, channel, 0, bufferSize, 0);
                }
            }
        }

        this->granulator_01_grains32_sampleOffset += this->granulator_01_grains32_curRate;
    }

    return {v, ph, ((bool)(bangDone) ? 1 : 0)};
}

bool granulator_01_grains_inactive(Index i) {
    switch ((int)i) {
    case 0:
        {
        return this->granulator_01_grains1_inactive();
        }
    case 1:
        {
        return this->granulator_01_grains2_inactive();
        }
    case 2:
        {
        return this->granulator_01_grains3_inactive();
        }
    case 3:
        {
        return this->granulator_01_grains4_inactive();
        }
    case 4:
        {
        return this->granulator_01_grains5_inactive();
        }
    case 5:
        {
        return this->granulator_01_grains6_inactive();
        }
    case 6:
        {
        return this->granulator_01_grains7_inactive();
        }
    case 7:
        {
        return this->granulator_01_grains8_inactive();
        }
    case 8:
        {
        return this->granulator_01_grains9_inactive();
        }
    case 9:
        {
        return this->granulator_01_grains10_inactive();
        }
    case 10:
        {
        return this->granulator_01_grains11_inactive();
        }
    case 11:
        {
        return this->granulator_01_grains12_inactive();
        }
    case 12:
        {
        return this->granulator_01_grains13_inactive();
        }
    case 13:
        {
        return this->granulator_01_grains14_inactive();
        }
    case 14:
        {
        return this->granulator_01_grains15_inactive();
        }
    case 15:
        {
        return this->granulator_01_grains16_inactive();
        }
    case 16:
        {
        return this->granulator_01_grains17_inactive();
        }
    case 17:
        {
        return this->granulator_01_grains18_inactive();
        }
    case 18:
        {
        return this->granulator_01_grains19_inactive();
        }
    case 19:
        {
        return this->granulator_01_grains20_inactive();
        }
    case 20:
        {
        return this->granulator_01_grains21_inactive();
        }
    case 21:
        {
        return this->granulator_01_grains22_inactive();
        }
    case 22:
        {
        return this->granulator_01_grains23_inactive();
        }
    case 23:
        {
        return this->granulator_01_grains24_inactive();
        }
    case 24:
        {
        return this->granulator_01_grains25_inactive();
        }
    case 25:
        {
        return this->granulator_01_grains26_inactive();
        }
    case 26:
        {
        return this->granulator_01_grains27_inactive();
        }
    case 27:
        {
        return this->granulator_01_grains28_inactive();
        }
    case 28:
        {
        return this->granulator_01_grains29_inactive();
        }
    case 29:
        {
        return this->granulator_01_grains30_inactive();
        }
    case 30:
        {
        return this->granulator_01_grains31_inactive();
        }
    default:
        {
        return this->granulator_01_grains32_inactive();
        }
    }

    return 0;
}

void granulator_01_grains_reset(Index i) {
    switch ((int)i) {
    case 0:
        {
        return this->granulator_01_grains1_reset();
        }
    case 1:
        {
        return this->granulator_01_grains2_reset();
        }
    case 2:
        {
        return this->granulator_01_grains3_reset();
        }
    case 3:
        {
        return this->granulator_01_grains4_reset();
        }
    case 4:
        {
        return this->granulator_01_grains5_reset();
        }
    case 5:
        {
        return this->granulator_01_grains6_reset();
        }
    case 6:
        {
        return this->granulator_01_grains7_reset();
        }
    case 7:
        {
        return this->granulator_01_grains8_reset();
        }
    case 8:
        {
        return this->granulator_01_grains9_reset();
        }
    case 9:
        {
        return this->granulator_01_grains10_reset();
        }
    case 10:
        {
        return this->granulator_01_grains11_reset();
        }
    case 11:
        {
        return this->granulator_01_grains12_reset();
        }
    case 12:
        {
        return this->granulator_01_grains13_reset();
        }
    case 13:
        {
        return this->granulator_01_grains14_reset();
        }
    case 14:
        {
        return this->granulator_01_grains15_reset();
        }
    case 15:
        {
        return this->granulator_01_grains16_reset();
        }
    case 16:
        {
        return this->granulator_01_grains17_reset();
        }
    case 17:
        {
        return this->granulator_01_grains18_reset();
        }
    case 18:
        {
        return this->granulator_01_grains19_reset();
        }
    case 19:
        {
        return this->granulator_01_grains20_reset();
        }
    case 20:
        {
        return this->granulator_01_grains21_reset();
        }
    case 21:
        {
        return this->granulator_01_grains22_reset();
        }
    case 22:
        {
        return this->granulator_01_grains23_reset();
        }
    case 23:
        {
        return this->granulator_01_grains24_reset();
        }
    case 24:
        {
        return this->granulator_01_grains25_reset();
        }
    case 25:
        {
        return this->granulator_01_grains26_reset();
        }
    case 26:
        {
        return this->granulator_01_grains27_reset();
        }
    case 27:
        {
        return this->granulator_01_grains28_reset();
        }
    case 28:
        {
        return this->granulator_01_grains29_reset();
        }
    case 29:
        {
        return this->granulator_01_grains30_reset();
        }
    case 30:
        {
        return this->granulator_01_grains31_reset();
        }
    default:
        {
        return this->granulator_01_grains32_reset();
        }
    }
}

template <typename T> inline array<number, 3> granulator_01_grains_next(
    Index i,
    T& buffer,
    number trigger,
    number start,
    number length,
    number rate,
    number channel
) {
    switch ((int)i) {
    case 0:
        {
        return this->granulator_01_grains1_next(buffer, trigger, start, length, rate, channel);
        }
    case 1:
        {
        return this->granulator_01_grains2_next(buffer, trigger, start, length, rate, channel);
        }
    case 2:
        {
        return this->granulator_01_grains3_next(buffer, trigger, start, length, rate, channel);
        }
    case 3:
        {
        return this->granulator_01_grains4_next(buffer, trigger, start, length, rate, channel);
        }
    case 4:
        {
        return this->granulator_01_grains5_next(buffer, trigger, start, length, rate, channel);
        }
    case 5:
        {
        return this->granulator_01_grains6_next(buffer, trigger, start, length, rate, channel);
        }
    case 6:
        {
        return this->granulator_01_grains7_next(buffer, trigger, start, length, rate, channel);
        }
    case 7:
        {
        return this->granulator_01_grains8_next(buffer, trigger, start, length, rate, channel);
        }
    case 8:
        {
        return this->granulator_01_grains9_next(buffer, trigger, start, length, rate, channel);
        }
    case 9:
        {
        return this->granulator_01_grains10_next(buffer, trigger, start, length, rate, channel);
        }
    case 10:
        {
        return this->granulator_01_grains11_next(buffer, trigger, start, length, rate, channel);
        }
    case 11:
        {
        return this->granulator_01_grains12_next(buffer, trigger, start, length, rate, channel);
        }
    case 12:
        {
        return this->granulator_01_grains13_next(buffer, trigger, start, length, rate, channel);
        }
    case 13:
        {
        return this->granulator_01_grains14_next(buffer, trigger, start, length, rate, channel);
        }
    case 14:
        {
        return this->granulator_01_grains15_next(buffer, trigger, start, length, rate, channel);
        }
    case 15:
        {
        return this->granulator_01_grains16_next(buffer, trigger, start, length, rate, channel);
        }
    case 16:
        {
        return this->granulator_01_grains17_next(buffer, trigger, start, length, rate, channel);
        }
    case 17:
        {
        return this->granulator_01_grains18_next(buffer, trigger, start, length, rate, channel);
        }
    case 18:
        {
        return this->granulator_01_grains19_next(buffer, trigger, start, length, rate, channel);
        }
    case 19:
        {
        return this->granulator_01_grains20_next(buffer, trigger, start, length, rate, channel);
        }
    case 20:
        {
        return this->granulator_01_grains21_next(buffer, trigger, start, length, rate, channel);
        }
    case 21:
        {
        return this->granulator_01_grains22_next(buffer, trigger, start, length, rate, channel);
        }
    case 22:
        {
        return this->granulator_01_grains23_next(buffer, trigger, start, length, rate, channel);
        }
    case 23:
        {
        return this->granulator_01_grains24_next(buffer, trigger, start, length, rate, channel);
        }
    case 24:
        {
        return this->granulator_01_grains25_next(buffer, trigger, start, length, rate, channel);
        }
    case 25:
        {
        return this->granulator_01_grains26_next(buffer, trigger, start, length, rate, channel);
        }
    case 26:
        {
        return this->granulator_01_grains27_next(buffer, trigger, start, length, rate, channel);
        }
    case 27:
        {
        return this->granulator_01_grains28_next(buffer, trigger, start, length, rate, channel);
        }
    case 28:
        {
        return this->granulator_01_grains29_next(buffer, trigger, start, length, rate, channel);
        }
    case 29:
        {
        return this->granulator_01_grains30_next(buffer, trigger, start, length, rate, channel);
        }
    case 30:
        {
        return this->granulator_01_grains31_next(buffer, trigger, start, length, rate, channel);
        }
    default:
        {
        return this->granulator_01_grains32_next(buffer, trigger, start, length, rate, channel);
        }
    }

    return 0;
}

void granulator_01_init() {
    this->granulator_01_edgedetect_init();

    for (Index i = 0; i < 32; i++) {
        for (Index j = 0; j < 2; j++) {
            this->granulator_01_grainPan[(Index)i][(Index)j] = 1;
        }
    }
}

void granulator_01_bufferUpdated() {
    number bufSr = this->granulator_01_buffer->getSampleRate();
    number bufSize = this->granulator_01_buffer->getSize();

    for (Index i = 0; i < 32; i++) {
        this->granulator_01_grains_reset(i);
    }

    {}
    {}
}

void granulator_01_onSampleRateChanged(number samplerate) {
    RNBO_UNUSED(samplerate);
    this->granulator_01_bufferUpdated();
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

number gaintilde_01_ramp_d_next(number x) {
    number temp = (number)(x - this->gaintilde_01_ramp_d_prev);
    this->gaintilde_01_ramp_d_prev = x;
    return temp;
}

void gaintilde_01_ramp_d_dspsetup() {
    this->gaintilde_01_ramp_d_reset();
}

void gaintilde_01_ramp_d_reset() {
    this->gaintilde_01_ramp_d_prev = 0;
}

number gaintilde_01_ramp_next(number x, number up, number down) {
    if (this->gaintilde_01_ramp_d_next(x) != 0.) {
        if (x > this->gaintilde_01_ramp_prev) {
            number _up = up;

            if (_up < 1)
                _up = 1;

            this->gaintilde_01_ramp_index = _up;
            this->gaintilde_01_ramp_increment = (x - this->gaintilde_01_ramp_prev) / _up;
        } else if (x < this->gaintilde_01_ramp_prev) {
            number _down = down;

            if (_down < 1)
                _down = 1;

            this->gaintilde_01_ramp_index = _down;
            this->gaintilde_01_ramp_increment = (x - this->gaintilde_01_ramp_prev) / _down;
        }
    }

    if (this->gaintilde_01_ramp_index > 0) {
        this->gaintilde_01_ramp_prev += this->gaintilde_01_ramp_increment;
        this->gaintilde_01_ramp_index -= 1;
    } else {
        this->gaintilde_01_ramp_prev = x;
    }

    return this->gaintilde_01_ramp_prev;
}

void gaintilde_01_ramp_reset() {
    this->gaintilde_01_ramp_prev = 0;
    this->gaintilde_01_ramp_index = 0;
    this->gaintilde_01_ramp_increment = 0;
    this->gaintilde_01_ramp_d_reset();
}

void gaintilde_01_init() {
    this->gaintilde_01_loginc = rnbo_log(1.072);
    this->gaintilde_01_zval = 7.943 * rnbo_exp(-((158 - 1) * this->gaintilde_01_loginc));
}

void gaintilde_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->gaintilde_01_value;
}

void gaintilde_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->gaintilde_01_value_set(preset["value"]);
}

void gaintilde_01_dspsetup(bool force) {
    if ((bool)(this->gaintilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gaintilde_01_setupDone = true;
    this->gaintilde_01_ramp_d_dspsetup();
}

number gaintilde_02_ramp_d_next(number x) {
    number temp = (number)(x - this->gaintilde_02_ramp_d_prev);
    this->gaintilde_02_ramp_d_prev = x;
    return temp;
}

void gaintilde_02_ramp_d_dspsetup() {
    this->gaintilde_02_ramp_d_reset();
}

void gaintilde_02_ramp_d_reset() {
    this->gaintilde_02_ramp_d_prev = 0;
}

number gaintilde_02_ramp_next(number x, number up, number down) {
    if (this->gaintilde_02_ramp_d_next(x) != 0.) {
        if (x > this->gaintilde_02_ramp_prev) {
            number _up = up;

            if (_up < 1)
                _up = 1;

            this->gaintilde_02_ramp_index = _up;
            this->gaintilde_02_ramp_increment = (x - this->gaintilde_02_ramp_prev) / _up;
        } else if (x < this->gaintilde_02_ramp_prev) {
            number _down = down;

            if (_down < 1)
                _down = 1;

            this->gaintilde_02_ramp_index = _down;
            this->gaintilde_02_ramp_increment = (x - this->gaintilde_02_ramp_prev) / _down;
        }
    }

    if (this->gaintilde_02_ramp_index > 0) {
        this->gaintilde_02_ramp_prev += this->gaintilde_02_ramp_increment;
        this->gaintilde_02_ramp_index -= 1;
    } else {
        this->gaintilde_02_ramp_prev = x;
    }

    return this->gaintilde_02_ramp_prev;
}

void gaintilde_02_ramp_reset() {
    this->gaintilde_02_ramp_prev = 0;
    this->gaintilde_02_ramp_index = 0;
    this->gaintilde_02_ramp_increment = 0;
    this->gaintilde_02_ramp_d_reset();
}

void gaintilde_02_init() {
    this->gaintilde_02_loginc = rnbo_log(1.072);
    this->gaintilde_02_zval = 7.943 * rnbo_exp(-((158 - 1) * this->gaintilde_02_loginc));
}

void gaintilde_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->gaintilde_02_value;
}

void gaintilde_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->gaintilde_02_value_set(preset["value"]);
}

void gaintilde_02_dspsetup(bool force) {
    if ((bool)(this->gaintilde_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gaintilde_02_setupDone = true;
    this->gaintilde_02_ramp_d_dspsetup();
}

void ip_01_init() {
    this->ip_01_lastValue = this->ip_01_value;
}

void ip_01_fillSigBuf() {
    if ((bool)(this->ip_01_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_01_lastIndex); i < k; i++) {
            if (this->ip_01_resetCount > 0) {
                this->ip_01_sigbuf[(Index)i] = 1;
                this->ip_01_resetCount--;
            } else {
                this->ip_01_sigbuf[(Index)i] = this->ip_01_lastValue;
            }
        }

        this->ip_01_lastIndex = k;
    }
}

void ip_01_dspsetup(bool force) {
    if ((bool)(this->ip_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_01_lastIndex = 0;
    this->ip_01_setupDone = true;
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void ip_02_init() {
    this->ip_02_lastValue = this->ip_02_value;
}

void ip_02_dspsetup(bool force) {
    if ((bool)(this->ip_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_02_lastIndex = 0;
    this->ip_02_setupDone = true;
}

void scopetilde_01_updateEffectiveCount() {
    number effectiveCount = 256 * 1 + 256 * 0;
    this->scopetilde_01_effectiveCount = this->maximum(effectiveCount, 256);
}

void scopetilde_01_dspsetup(bool force) {
    if ((bool)(this->scopetilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    {
        this->scopetilde_01_mode = 1;
    }

    this->getEngine()->sendListMessage(
        TAG("setup"),
        TAG("scope~_obj-7"),
        {1, 1, this->samplerate(), 0, 1, 0, 0, 128, this->scopetilde_01_mode},
        this->_currentTime
    );;

    this->scopetilde_01_updateEffectiveCount();
    this->scopetilde_01_setupDone = true;
}

void data_01_init() {
    this->data_01_buffer->setWantsFill(true);
}

Index data_01_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    number size = 0;
    return (Index)(size);
}

void data_01_dspsetup(bool force) {
    if ((bool)(this->data_01_setupDone) && (bool)(!(bool)(force)))
        return;

    if (this->data_01_sizemode == 2) {
        this->data_01_buffer = this->data_01_buffer->setSize((Index)(this->mstosamps(this->data_01_sizems)));
        updateDataRef(this, this->data_01_buffer);
    } else if (this->data_01_sizemode == 3) {
        this->data_01_buffer = this->data_01_buffer->setSize(this->data_01_evaluateSizeExpr(this->samplerate(), this->vectorsize()));
        updateDataRef(this, this->data_01_buffer);
    }

    this->data_01_setupDone = true;
}

void data_01_bufferUpdated() {
    this->data_01_report();
}

void data_01_report() {
    this->data_01_srout_set(this->data_01_buffer->getSampleRate());
    this->data_01_chanout_set(this->data_01_buffer->getChannels());
    this->data_01_sizeout_set(this->data_01_buffer->getSize());
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void scopetilde_02_updateEffectiveCount() {
    number effectiveCount = 42 * 1 + 42 * 0;
    this->scopetilde_02_effectiveCount = this->maximum(effectiveCount, 42);
}

void scopetilde_02_dspsetup(bool force) {
    if ((bool)(this->scopetilde_02_setupDone) && (bool)(!(bool)(force)))
        return;

    {
        this->scopetilde_02_mode = 1;
    }

    this->getEngine()->sendListMessage(
        TAG("setup"),
        TAG("scope~_obj-10"),
        {1, 1, this->samplerate(), 0, 1, 0, 0, 128, this->scopetilde_02_mode},
        this->_currentTime
    );;

    this->scopetilde_02_updateEffectiveCount();
    this->scopetilde_02_setupDone = true;
}

void adsr_01_dspsetup(bool force) {
    if ((bool)(this->adsr_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->adsr_01_mspersamp = (number)1000 / this->sr;
    this->adsr_01_setupDone = true;
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void numberobj_01_init() {
    this->numberobj_01_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-34"), 1, this->_currentTime);
}

void numberobj_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_01_value;
}

void numberobj_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_01_value_set(preset["value"]);
}

list midiparse_01_parser_next(number midiin) {
    int midivalue = (int)(trunc(midiin));
    list result = list(-1);
    number resetByte1 = false;

    if (midivalue > 127) {
        this->midiparse_01_parser_status = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0xFF);
        this->midiparse_01_parser_byte1 = -1;
    } else {
        switch ((int)((BinOpInt)this->midiparse_01_parser_status & (BinOpInt)0xF0)) {
        case 0xB0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    2,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xA0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    1,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xE0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                number pitchbend;

                {
                    number val = this->midiparse_01_parser_byte1 + ((BinOpInt)(((BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F)) << imod_nocast((UBinOpInt)7, 32)));

                    {
                        val -= 0x2000;
                        pitchbend = val / (number)8192;
                    }
                }

                result = {
                    5,
                    pitchbend,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xD0:
            {
            result = {
                4,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x90:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xC0:
            {
            result = {
                3,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x80:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    0,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        default:
            {
            result = {-1};
            }
        }
    }

    if (result->length > 1) {
        result->push(this->midiparse_01_parser_status);
        result->push(this->midiparse_01_parser_byte1);
        result->push(midivalue);
    }

    if ((bool)(resetByte1)) {
        this->midiparse_01_parser_byte1 = -1;
    }

    return result;
}

void midiparse_01_parser_reset() {
    this->midiparse_01_parser_status = -1;
    this->midiparse_01_parser_byte1 = -1;
}

void numberobj_02_init() {
    this->numberobj_02_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-33"), 1, this->_currentTime);
}

void numberobj_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_02_value;
}

void numberobj_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_02_value_set(preset["value"]);
}

void param_08_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_08_value;
}

void param_08_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_08_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    param_01_value = 124;
    param_02_value = 6;
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0.5;
    phasor_01_freq = 6;
    granulator_01_start = 0;
    granulator_01_length = 2205;
    granulator_01_rate = 1;
    granulator_01_bchan = 0;
    granulator_01_limit = 32;
    granulator_01_panpos = 0.5;
    param_03_value = 0;
    mstosamps_tilde_01_ms = 0;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 0;
    mstosamps_tilde_02_ms = 0;
    param_04_value = 300;
    gaintilde_01_input_number = 0;
    gaintilde_01_value = 0;
    gaintilde_01_interp = 10;
    gaintilde_02_input_number = 0;
    gaintilde_02_value = 0;
    gaintilde_02_interp = 10;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0;
    ip_01_value = 0;
    ip_01_impulse = 0;
    dspexpr_04_in1 = 0;
    dspexpr_04_in2 = 0;
    param_05_value = 1;
    ip_02_value = 0.5;
    ip_02_impulse = 0;
    data_01_sizeout = 0;
    data_01_size = 0;
    data_01_sizems = 0;
    data_01_normalize = 0.995;
    data_01_channels = 1;
    param_06_value = 0;
    adsr_01_trigger_number = 0;
    adsr_01_attack = 80;
    adsr_01_decay = 0;
    adsr_01_sustain = 1;
    adsr_01_release = 1000;
    adsr_01_legato = 0;
    adsr_01_maxsustain = -1;
    param_07_value = 10;
    numberobj_01_value = 0;
    numberobj_01_value_setter(numberobj_01_value);
    unpack_01_out1 = 0;
    unpack_01_out2 = 0;
    numberobj_02_value = 0;
    numberobj_02_value_setter(numberobj_02_value);
    param_08_value = 500;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    signals[4] = nullptr;
    signals[5] = nullptr;
    signals[6] = nullptr;
    signals[7] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    phasor_01_sigbuf = nullptr;
    phasor_01_lastLockedPhase = 0;
    phasor_01_conv = 0;
    phasor_01_ph_currentPhase = 0;
    phasor_01_ph_conv = 0;
    phasor_01_setupDone = false;
    granulator_01_startMul = 1;
    granulator_01_lengthMul = 1;
    granulator_01_limitLast = 0;
    granulator_01_edgedetect_active = false;
    granulator_01_grains1_sampleOffset = 0;
    granulator_01_grains1_curRate = 1;
    granulator_01_grains1_curStart = 0;
    granulator_01_grains1_curLen = 1;
    granulator_01_grains1_active = false;
    granulator_01_grains1_backwards = false;
    granulator_01_grains2_sampleOffset = 0;
    granulator_01_grains2_curRate = 1;
    granulator_01_grains2_curStart = 0;
    granulator_01_grains2_curLen = 1;
    granulator_01_grains2_active = false;
    granulator_01_grains2_backwards = false;
    granulator_01_grains3_sampleOffset = 0;
    granulator_01_grains3_curRate = 1;
    granulator_01_grains3_curStart = 0;
    granulator_01_grains3_curLen = 1;
    granulator_01_grains3_active = false;
    granulator_01_grains3_backwards = false;
    granulator_01_grains4_sampleOffset = 0;
    granulator_01_grains4_curRate = 1;
    granulator_01_grains4_curStart = 0;
    granulator_01_grains4_curLen = 1;
    granulator_01_grains4_active = false;
    granulator_01_grains4_backwards = false;
    granulator_01_grains5_sampleOffset = 0;
    granulator_01_grains5_curRate = 1;
    granulator_01_grains5_curStart = 0;
    granulator_01_grains5_curLen = 1;
    granulator_01_grains5_active = false;
    granulator_01_grains5_backwards = false;
    granulator_01_grains6_sampleOffset = 0;
    granulator_01_grains6_curRate = 1;
    granulator_01_grains6_curStart = 0;
    granulator_01_grains6_curLen = 1;
    granulator_01_grains6_active = false;
    granulator_01_grains6_backwards = false;
    granulator_01_grains7_sampleOffset = 0;
    granulator_01_grains7_curRate = 1;
    granulator_01_grains7_curStart = 0;
    granulator_01_grains7_curLen = 1;
    granulator_01_grains7_active = false;
    granulator_01_grains7_backwards = false;
    granulator_01_grains8_sampleOffset = 0;
    granulator_01_grains8_curRate = 1;
    granulator_01_grains8_curStart = 0;
    granulator_01_grains8_curLen = 1;
    granulator_01_grains8_active = false;
    granulator_01_grains8_backwards = false;
    granulator_01_grains9_sampleOffset = 0;
    granulator_01_grains9_curRate = 1;
    granulator_01_grains9_curStart = 0;
    granulator_01_grains9_curLen = 1;
    granulator_01_grains9_active = false;
    granulator_01_grains9_backwards = false;
    granulator_01_grains10_sampleOffset = 0;
    granulator_01_grains10_curRate = 1;
    granulator_01_grains10_curStart = 0;
    granulator_01_grains10_curLen = 1;
    granulator_01_grains10_active = false;
    granulator_01_grains10_backwards = false;
    granulator_01_grains11_sampleOffset = 0;
    granulator_01_grains11_curRate = 1;
    granulator_01_grains11_curStart = 0;
    granulator_01_grains11_curLen = 1;
    granulator_01_grains11_active = false;
    granulator_01_grains11_backwards = false;
    granulator_01_grains12_sampleOffset = 0;
    granulator_01_grains12_curRate = 1;
    granulator_01_grains12_curStart = 0;
    granulator_01_grains12_curLen = 1;
    granulator_01_grains12_active = false;
    granulator_01_grains12_backwards = false;
    granulator_01_grains13_sampleOffset = 0;
    granulator_01_grains13_curRate = 1;
    granulator_01_grains13_curStart = 0;
    granulator_01_grains13_curLen = 1;
    granulator_01_grains13_active = false;
    granulator_01_grains13_backwards = false;
    granulator_01_grains14_sampleOffset = 0;
    granulator_01_grains14_curRate = 1;
    granulator_01_grains14_curStart = 0;
    granulator_01_grains14_curLen = 1;
    granulator_01_grains14_active = false;
    granulator_01_grains14_backwards = false;
    granulator_01_grains15_sampleOffset = 0;
    granulator_01_grains15_curRate = 1;
    granulator_01_grains15_curStart = 0;
    granulator_01_grains15_curLen = 1;
    granulator_01_grains15_active = false;
    granulator_01_grains15_backwards = false;
    granulator_01_grains16_sampleOffset = 0;
    granulator_01_grains16_curRate = 1;
    granulator_01_grains16_curStart = 0;
    granulator_01_grains16_curLen = 1;
    granulator_01_grains16_active = false;
    granulator_01_grains16_backwards = false;
    granulator_01_grains17_sampleOffset = 0;
    granulator_01_grains17_curRate = 1;
    granulator_01_grains17_curStart = 0;
    granulator_01_grains17_curLen = 1;
    granulator_01_grains17_active = false;
    granulator_01_grains17_backwards = false;
    granulator_01_grains18_sampleOffset = 0;
    granulator_01_grains18_curRate = 1;
    granulator_01_grains18_curStart = 0;
    granulator_01_grains18_curLen = 1;
    granulator_01_grains18_active = false;
    granulator_01_grains18_backwards = false;
    granulator_01_grains19_sampleOffset = 0;
    granulator_01_grains19_curRate = 1;
    granulator_01_grains19_curStart = 0;
    granulator_01_grains19_curLen = 1;
    granulator_01_grains19_active = false;
    granulator_01_grains19_backwards = false;
    granulator_01_grains20_sampleOffset = 0;
    granulator_01_grains20_curRate = 1;
    granulator_01_grains20_curStart = 0;
    granulator_01_grains20_curLen = 1;
    granulator_01_grains20_active = false;
    granulator_01_grains20_backwards = false;
    granulator_01_grains21_sampleOffset = 0;
    granulator_01_grains21_curRate = 1;
    granulator_01_grains21_curStart = 0;
    granulator_01_grains21_curLen = 1;
    granulator_01_grains21_active = false;
    granulator_01_grains21_backwards = false;
    granulator_01_grains22_sampleOffset = 0;
    granulator_01_grains22_curRate = 1;
    granulator_01_grains22_curStart = 0;
    granulator_01_grains22_curLen = 1;
    granulator_01_grains22_active = false;
    granulator_01_grains22_backwards = false;
    granulator_01_grains23_sampleOffset = 0;
    granulator_01_grains23_curRate = 1;
    granulator_01_grains23_curStart = 0;
    granulator_01_grains23_curLen = 1;
    granulator_01_grains23_active = false;
    granulator_01_grains23_backwards = false;
    granulator_01_grains24_sampleOffset = 0;
    granulator_01_grains24_curRate = 1;
    granulator_01_grains24_curStart = 0;
    granulator_01_grains24_curLen = 1;
    granulator_01_grains24_active = false;
    granulator_01_grains24_backwards = false;
    granulator_01_grains25_sampleOffset = 0;
    granulator_01_grains25_curRate = 1;
    granulator_01_grains25_curStart = 0;
    granulator_01_grains25_curLen = 1;
    granulator_01_grains25_active = false;
    granulator_01_grains25_backwards = false;
    granulator_01_grains26_sampleOffset = 0;
    granulator_01_grains26_curRate = 1;
    granulator_01_grains26_curStart = 0;
    granulator_01_grains26_curLen = 1;
    granulator_01_grains26_active = false;
    granulator_01_grains26_backwards = false;
    granulator_01_grains27_sampleOffset = 0;
    granulator_01_grains27_curRate = 1;
    granulator_01_grains27_curStart = 0;
    granulator_01_grains27_curLen = 1;
    granulator_01_grains27_active = false;
    granulator_01_grains27_backwards = false;
    granulator_01_grains28_sampleOffset = 0;
    granulator_01_grains28_curRate = 1;
    granulator_01_grains28_curStart = 0;
    granulator_01_grains28_curLen = 1;
    granulator_01_grains28_active = false;
    granulator_01_grains28_backwards = false;
    granulator_01_grains29_sampleOffset = 0;
    granulator_01_grains29_curRate = 1;
    granulator_01_grains29_curStart = 0;
    granulator_01_grains29_curLen = 1;
    granulator_01_grains29_active = false;
    granulator_01_grains29_backwards = false;
    granulator_01_grains30_sampleOffset = 0;
    granulator_01_grains30_curRate = 1;
    granulator_01_grains30_curStart = 0;
    granulator_01_grains30_curLen = 1;
    granulator_01_grains30_active = false;
    granulator_01_grains30_backwards = false;
    granulator_01_grains31_sampleOffset = 0;
    granulator_01_grains31_curRate = 1;
    granulator_01_grains31_curStart = 0;
    granulator_01_grains31_curLen = 1;
    granulator_01_grains31_active = false;
    granulator_01_grains31_backwards = false;
    granulator_01_grains32_sampleOffset = 0;
    granulator_01_grains32_curRate = 1;
    granulator_01_grains32_curStart = 0;
    granulator_01_grains32_curLen = 1;
    granulator_01_grains32_active = false;
    granulator_01_grains32_backwards = false;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    gaintilde_01_lastValue = 0;
    gaintilde_01_loginc = 1;
    gaintilde_01_zval = 0;
    gaintilde_01_ramp_d_prev = 0;
    gaintilde_01_ramp_prev = 0;
    gaintilde_01_ramp_index = 0;
    gaintilde_01_ramp_increment = 0;
    gaintilde_01_setupDone = false;
    gaintilde_02_lastValue = 0;
    gaintilde_02_loginc = 1;
    gaintilde_02_zval = 0;
    gaintilde_02_ramp_d_prev = 0;
    gaintilde_02_ramp_prev = 0;
    gaintilde_02_ramp_index = 0;
    gaintilde_02_ramp_increment = 0;
    gaintilde_02_setupDone = false;
    ip_01_lastIndex = 0;
    ip_01_lastValue = 0;
    ip_01_resetCount = 0;
    ip_01_sigbuf = nullptr;
    ip_01_setupDone = false;
    param_05_lastValue = 0;
    ip_02_lastIndex = 0;
    ip_02_lastValue = 0;
    ip_02_resetCount = 0;
    ip_02_sigbuf = nullptr;
    ip_02_setupDone = false;
    scopetilde_01_lastValue = 0;
    scopetilde_01_effectiveCount = 256;
    scopetilde_01_xsign = 1;
    scopetilde_01_ysign = 1;
    scopetilde_01_mode = 0;
    scopetilde_01_setupDone = false;
    data_01_sizemode = 0;
    data_01_setupDone = false;
    param_06_lastValue = 0;
    scopetilde_02_lastValue = 0;
    scopetilde_02_effectiveCount = 256;
    scopetilde_02_xsign = 1;
    scopetilde_02_ysign = 1;
    scopetilde_02_mode = 0;
    scopetilde_02_setupDone = false;
    adsr_01_phase = 3;
    adsr_01_mspersamp = 0;
    adsr_01_time = 0;
    adsr_01_lastTriggerVal = 0;
    adsr_01_amplitude = 0;
    adsr_01_outval = 0;
    adsr_01_startingpoint = 0;
    adsr_01_triggerBuf = nullptr;
    adsr_01_triggerValueBuf = nullptr;
    adsr_01_setupDone = false;
    param_07_lastValue = 0;
    numberobj_01_currentFormat = 6;
    numberobj_01_lastValue = 0;
    midiparse_01_parser_status = -1;
    midiparse_01_parser_byte1 = -1;
    numberobj_02_currentFormat = 6;
    numberobj_02_lastValue = 0;
    param_08_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number param_01_value;
    number param_02_value;
    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number phasor_01_freq;
    number granulator_01_start;
    number granulator_01_length;
    number granulator_01_rate;
    number granulator_01_bchan;
    number granulator_01_limit;
    number granulator_01_panpos;
    number param_03_value;
    number mstosamps_tilde_01_ms;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number mstosamps_tilde_02_ms;
    number param_04_value;
    number gaintilde_01_input_number;
    number gaintilde_01_value;
    number gaintilde_01_interp;
    number gaintilde_02_input_number;
    number gaintilde_02_value;
    number gaintilde_02_interp;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number ip_01_value;
    number ip_01_impulse;
    number dspexpr_04_in1;
    number dspexpr_04_in2;
    number param_05_value;
    number ip_02_value;
    number ip_02_impulse;
    number data_01_sizeout;
    number data_01_size;
    number data_01_sizems;
    number data_01_normalize;
    number data_01_channels;
    number param_06_value;
    number adsr_01_trigger_number;
    number adsr_01_attack;
    number adsr_01_decay;
    number adsr_01_sustain;
    number adsr_01_release;
    number adsr_01_legato;
    number adsr_01_maxsustain;
    number param_07_value;
    number numberobj_01_value;
    number unpack_01_out1;
    number unpack_01_out2;
    number numberobj_02_value;
    number param_08_value;
    MillisecondTime _currentTime;
    UInt64 audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[8];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number param_01_lastValue;
    number param_02_lastValue;
    signal phasor_01_sigbuf;
    number phasor_01_lastLockedPhase;
    number phasor_01_conv;
    number phasor_01_ph_currentPhase;
    number phasor_01_ph_conv;
    bool phasor_01_setupDone;
    Float32BufferRef granulator_01_buffer;
    number granulator_01_grainPan[32][2] = { };
    number granulator_01_startMul;
    number granulator_01_lengthMul;
    int granulator_01_limitLast;
    bool granulator_01_edgedetect_active;
    number granulator_01_grains1_sampleOffset;
    number granulator_01_grains1_curRate;
    number granulator_01_grains1_curStart;
    number granulator_01_grains1_curLen;
    bool granulator_01_grains1_active;
    bool granulator_01_grains1_backwards;
    number granulator_01_grains2_sampleOffset;
    number granulator_01_grains2_curRate;
    number granulator_01_grains2_curStart;
    number granulator_01_grains2_curLen;
    bool granulator_01_grains2_active;
    bool granulator_01_grains2_backwards;
    number granulator_01_grains3_sampleOffset;
    number granulator_01_grains3_curRate;
    number granulator_01_grains3_curStart;
    number granulator_01_grains3_curLen;
    bool granulator_01_grains3_active;
    bool granulator_01_grains3_backwards;
    number granulator_01_grains4_sampleOffset;
    number granulator_01_grains4_curRate;
    number granulator_01_grains4_curStart;
    number granulator_01_grains4_curLen;
    bool granulator_01_grains4_active;
    bool granulator_01_grains4_backwards;
    number granulator_01_grains5_sampleOffset;
    number granulator_01_grains5_curRate;
    number granulator_01_grains5_curStart;
    number granulator_01_grains5_curLen;
    bool granulator_01_grains5_active;
    bool granulator_01_grains5_backwards;
    number granulator_01_grains6_sampleOffset;
    number granulator_01_grains6_curRate;
    number granulator_01_grains6_curStart;
    number granulator_01_grains6_curLen;
    bool granulator_01_grains6_active;
    bool granulator_01_grains6_backwards;
    number granulator_01_grains7_sampleOffset;
    number granulator_01_grains7_curRate;
    number granulator_01_grains7_curStart;
    number granulator_01_grains7_curLen;
    bool granulator_01_grains7_active;
    bool granulator_01_grains7_backwards;
    number granulator_01_grains8_sampleOffset;
    number granulator_01_grains8_curRate;
    number granulator_01_grains8_curStart;
    number granulator_01_grains8_curLen;
    bool granulator_01_grains8_active;
    bool granulator_01_grains8_backwards;
    number granulator_01_grains9_sampleOffset;
    number granulator_01_grains9_curRate;
    number granulator_01_grains9_curStart;
    number granulator_01_grains9_curLen;
    bool granulator_01_grains9_active;
    bool granulator_01_grains9_backwards;
    number granulator_01_grains10_sampleOffset;
    number granulator_01_grains10_curRate;
    number granulator_01_grains10_curStart;
    number granulator_01_grains10_curLen;
    bool granulator_01_grains10_active;
    bool granulator_01_grains10_backwards;
    number granulator_01_grains11_sampleOffset;
    number granulator_01_grains11_curRate;
    number granulator_01_grains11_curStart;
    number granulator_01_grains11_curLen;
    bool granulator_01_grains11_active;
    bool granulator_01_grains11_backwards;
    number granulator_01_grains12_sampleOffset;
    number granulator_01_grains12_curRate;
    number granulator_01_grains12_curStart;
    number granulator_01_grains12_curLen;
    bool granulator_01_grains12_active;
    bool granulator_01_grains12_backwards;
    number granulator_01_grains13_sampleOffset;
    number granulator_01_grains13_curRate;
    number granulator_01_grains13_curStart;
    number granulator_01_grains13_curLen;
    bool granulator_01_grains13_active;
    bool granulator_01_grains13_backwards;
    number granulator_01_grains14_sampleOffset;
    number granulator_01_grains14_curRate;
    number granulator_01_grains14_curStart;
    number granulator_01_grains14_curLen;
    bool granulator_01_grains14_active;
    bool granulator_01_grains14_backwards;
    number granulator_01_grains15_sampleOffset;
    number granulator_01_grains15_curRate;
    number granulator_01_grains15_curStart;
    number granulator_01_grains15_curLen;
    bool granulator_01_grains15_active;
    bool granulator_01_grains15_backwards;
    number granulator_01_grains16_sampleOffset;
    number granulator_01_grains16_curRate;
    number granulator_01_grains16_curStart;
    number granulator_01_grains16_curLen;
    bool granulator_01_grains16_active;
    bool granulator_01_grains16_backwards;
    number granulator_01_grains17_sampleOffset;
    number granulator_01_grains17_curRate;
    number granulator_01_grains17_curStart;
    number granulator_01_grains17_curLen;
    bool granulator_01_grains17_active;
    bool granulator_01_grains17_backwards;
    number granulator_01_grains18_sampleOffset;
    number granulator_01_grains18_curRate;
    number granulator_01_grains18_curStart;
    number granulator_01_grains18_curLen;
    bool granulator_01_grains18_active;
    bool granulator_01_grains18_backwards;
    number granulator_01_grains19_sampleOffset;
    number granulator_01_grains19_curRate;
    number granulator_01_grains19_curStart;
    number granulator_01_grains19_curLen;
    bool granulator_01_grains19_active;
    bool granulator_01_grains19_backwards;
    number granulator_01_grains20_sampleOffset;
    number granulator_01_grains20_curRate;
    number granulator_01_grains20_curStart;
    number granulator_01_grains20_curLen;
    bool granulator_01_grains20_active;
    bool granulator_01_grains20_backwards;
    number granulator_01_grains21_sampleOffset;
    number granulator_01_grains21_curRate;
    number granulator_01_grains21_curStart;
    number granulator_01_grains21_curLen;
    bool granulator_01_grains21_active;
    bool granulator_01_grains21_backwards;
    number granulator_01_grains22_sampleOffset;
    number granulator_01_grains22_curRate;
    number granulator_01_grains22_curStart;
    number granulator_01_grains22_curLen;
    bool granulator_01_grains22_active;
    bool granulator_01_grains22_backwards;
    number granulator_01_grains23_sampleOffset;
    number granulator_01_grains23_curRate;
    number granulator_01_grains23_curStart;
    number granulator_01_grains23_curLen;
    bool granulator_01_grains23_active;
    bool granulator_01_grains23_backwards;
    number granulator_01_grains24_sampleOffset;
    number granulator_01_grains24_curRate;
    number granulator_01_grains24_curStart;
    number granulator_01_grains24_curLen;
    bool granulator_01_grains24_active;
    bool granulator_01_grains24_backwards;
    number granulator_01_grains25_sampleOffset;
    number granulator_01_grains25_curRate;
    number granulator_01_grains25_curStart;
    number granulator_01_grains25_curLen;
    bool granulator_01_grains25_active;
    bool granulator_01_grains25_backwards;
    number granulator_01_grains26_sampleOffset;
    number granulator_01_grains26_curRate;
    number granulator_01_grains26_curStart;
    number granulator_01_grains26_curLen;
    bool granulator_01_grains26_active;
    bool granulator_01_grains26_backwards;
    number granulator_01_grains27_sampleOffset;
    number granulator_01_grains27_curRate;
    number granulator_01_grains27_curStart;
    number granulator_01_grains27_curLen;
    bool granulator_01_grains27_active;
    bool granulator_01_grains27_backwards;
    number granulator_01_grains28_sampleOffset;
    number granulator_01_grains28_curRate;
    number granulator_01_grains28_curStart;
    number granulator_01_grains28_curLen;
    bool granulator_01_grains28_active;
    bool granulator_01_grains28_backwards;
    number granulator_01_grains29_sampleOffset;
    number granulator_01_grains29_curRate;
    number granulator_01_grains29_curStart;
    number granulator_01_grains29_curLen;
    bool granulator_01_grains29_active;
    bool granulator_01_grains29_backwards;
    number granulator_01_grains30_sampleOffset;
    number granulator_01_grains30_curRate;
    number granulator_01_grains30_curStart;
    number granulator_01_grains30_curLen;
    bool granulator_01_grains30_active;
    bool granulator_01_grains30_backwards;
    number granulator_01_grains31_sampleOffset;
    number granulator_01_grains31_curRate;
    number granulator_01_grains31_curStart;
    number granulator_01_grains31_curLen;
    bool granulator_01_grains31_active;
    bool granulator_01_grains31_backwards;
    number granulator_01_grains32_sampleOffset;
    number granulator_01_grains32_curRate;
    number granulator_01_grains32_curStart;
    number granulator_01_grains32_curLen;
    bool granulator_01_grains32_active;
    bool granulator_01_grains32_backwards;
    number param_03_lastValue;
    number param_04_lastValue;
    number gaintilde_01_lastValue;
    number gaintilde_01_loginc;
    number gaintilde_01_zval;
    number gaintilde_01_ramp_d_prev;
    number gaintilde_01_ramp_prev;
    number gaintilde_01_ramp_index;
    number gaintilde_01_ramp_increment;
    bool gaintilde_01_setupDone;
    number gaintilde_02_lastValue;
    number gaintilde_02_loginc;
    number gaintilde_02_zval;
    number gaintilde_02_ramp_d_prev;
    number gaintilde_02_ramp_prev;
    number gaintilde_02_ramp_index;
    number gaintilde_02_ramp_increment;
    bool gaintilde_02_setupDone;
    SampleIndex ip_01_lastIndex;
    number ip_01_lastValue;
    SampleIndex ip_01_resetCount;
    signal ip_01_sigbuf;
    bool ip_01_setupDone;
    number param_05_lastValue;
    SampleIndex ip_02_lastIndex;
    number ip_02_lastValue;
    SampleIndex ip_02_resetCount;
    signal ip_02_sigbuf;
    bool ip_02_setupDone;
    number scopetilde_01_lastValue;
    number scopetilde_01_effectiveCount;
    number scopetilde_01_xmonitorvalue;
    number scopetilde_01_ymonitorvalue;
    list scopetilde_01_monitorbuffer;
    number scopetilde_01_xsign;
    number scopetilde_01_ysign;
    Int scopetilde_01_mode;
    bool scopetilde_01_setupDone;
    Float32BufferRef data_01_buffer;
    Int data_01_sizemode;
    bool data_01_setupDone;
    number param_06_lastValue;
    number scopetilde_02_lastValue;
    number scopetilde_02_effectiveCount;
    number scopetilde_02_xmonitorvalue;
    number scopetilde_02_ymonitorvalue;
    list scopetilde_02_monitorbuffer;
    number scopetilde_02_xsign;
    number scopetilde_02_ysign;
    Int scopetilde_02_mode;
    bool scopetilde_02_setupDone;
    Int adsr_01_phase;
    number adsr_01_mspersamp;
    number adsr_01_time;
    number adsr_01_lastTriggerVal;
    number adsr_01_amplitude;
    number adsr_01_outval;
    number adsr_01_startingpoint;
    signal adsr_01_triggerBuf;
    signal adsr_01_triggerValueBuf;
    bool adsr_01_setupDone;
    number param_07_lastValue;
    Int numberobj_01_currentFormat;
    number numberobj_01_lastValue;
    int midiparse_01_parser_status;
    int midiparse_01_parser_byte1;
    Int numberobj_02_currentFormat;
    number numberobj_02_lastValue;
    number param_08_lastValue;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef drums;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

