/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2025 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <inviwo/core/util/exception.h>
#include <inviwo/dicom/errorlogging.h>

#include <string>
#include <exception>
#include <ios>
#include <streambuf>
#include <ostream>
#include <iostream>
#include <cstdio>
#include <cstdarg>

#if defined(IVW_DEBUG)
// doesnt seem to work, use the cmake GDCM_ALWAYS_TRACE_MACRO flag instead
#define GDCM_ALWAYS_TRACE_MACRO
#endif

#include <warn/push>
#include <warn/ignore/all>
#include <tiff.h>
#include <tiffio.h>

#include <gdcmTrace.h>
#include <warn/pop>

namespace inviwo {

// =====================================================================
// gdcm writes its log messages to a std::ostream
// => we derive a logging class from std::ostream and pass
// the logmessages forward to Inviwos LogCentral interface.
// =====================================================================

/* @brief streambuffer class to store log messages and print them at end of line (\n)

no seeking supported!
'simple' output only as no buffer is provided via streambuf::(pub)setbuf()
internal a string is used
*/
template <LogLevel loglevel, class CharT, class Traits = std::char_traits<CharT>>
class StreambufLineout : public std::streambuf {
private:
    std::basic_string<CharT, Traits> buffer;

protected:
    // int overflow(int ch) override
    virtual int overflow(int ch) {
        try {
            if (ch == '\n') {
                if (!buffer.empty()) {
                    // at the end of a line we want to pass the message to Inviwo's logging system
                    inviwo::LogCentral::getPtr()->log("Gdcm Volume Importer", loglevel,
                                                      LogAudience::User, "<GDCM library>", "", 0,
                                                      buffer);
                }
                buffer.clear();
            } else if (!Traits::eq_int_type(ch, Traits::eof())) {
                buffer.push_back(static_cast<CharT>(ch));
            }
        } catch (std::exception& ex) {
            // error while logging, print what we've got so far
            inviwo::LogCentral::getPtr()->log("Gdcm Volume Importer", loglevel, LogAudience::User,
                                              "<GDCM library>", "", 0, buffer);
            buffer.clear();
            // and tell the user what hargspened
            LogError("Exception while logging Gdcm: " << ex.what());
            // return traits::eof on failure
            return Traits::eof();
        }
        // returning any value != traits::eof is fine on success, so invert eof
        return !(Traits::eof());
    }
};

/* @brief Gdcm log message => std::ostream => Inviwo LogCentral

simple line oriented logging ostream class utilizing StreambufLineout
not full-featured, \see StreambufLineout
TODO: check standard conformance
*/
template <LogLevel loglevel>
class IvwGdcmLogStream : public std::ostream {
public:
    // std::ios(0) constructor?
    IvwGdcmLogStream() : std::ostream(&lnstreambuf) {}

    virtual ~IvwGdcmLogStream() {
        // calls sync to clean things up
        lnstreambuf.pubsync();
    }

private:
    StreambufLineout<loglevel, char, std::char_traits<char>> lnstreambuf;
};

void enableGdcmLogging(LogVerbosity verbosity) {
    static IvwGdcmLogStream<LogLevel::Info> debug_log_stream;
    static IvwGdcmLogStream<LogLevel::Warn> warning_log_stream;
    static IvwGdcmLogStream<LogLevel::Error> error_log_stream;

    // reset logging/tracing in gdcm
    gdcm::Trace();

    switch (verbosity) {
        case LogVerbosity::Info:
            gdcm::Trace::DebugOn();
            gdcm::Trace::SetDebugStream(debug_log_stream);
            [[fallthrough]];
        case LogVerbosity::Warn:
            gdcm::Trace::WarningOn();
            gdcm::Trace::SetWarningStream(warning_log_stream);
            [[fallthrough]];
        case LogVerbosity::Error:
            gdcm::Trace::ErrorOn();
            gdcm::Trace::SetErrorStream(error_log_stream);
            break;
        case LogVerbosity::None:
        default:
            break;
    }

    /*
#if defined(IVW_DEBUG)
    gdcmDebugMacro("GDCM debug output available.");
    gdcmWarningMacro("GDCM warning output available.");
    gdcmErrorMacro("GDCM error output available.");
#endif
    */
}

void tiffErrorHandler(const char* module, const char* fmt, va_list args) {
    char buffer[512];
    int res = vsnprintf(buffer, 512, fmt, args);

    std::string err_str;
    if (res < 1) {
        err_str = std::string("got libtiff error from ") + module + " but snprintf failed.";
    } else {
        err_str = buffer;
    }

    inviwo::LogCentral::getPtr()->log("Gdcm Volume Importer", LogLevel::Error, LogAudience::User,
                                      "<libtiff>", module, 0, err_str);
}

void tiffWarningHandler(const char* module, const char* fmt, va_list args) {
    char buffer[512];
    int res = snprintf(buffer, 512, fmt, args);

    std::string err_str;
    if (res < 1) {
        err_str = std::string("got libtiff warning from ") + module + " but snprintf failed.";
    } else {
        err_str = buffer;
    }

    inviwo::LogCentral::getPtr()->log("Gdcm Volume Importer", LogLevel::Warn, LogAudience::User,
                                      "<libtiff>", module, 0, err_str);
}

void enableTiffLogging(LogVerbosity verbosity) {
    switch (verbosity) {
        case LogVerbosity::Info:
        case LogVerbosity::Warn:
            TIFFSetWarningHandler((TIFFErrorHandler)tiffWarningHandler);
            [[fallthrough]];
        case LogVerbosity::Error:
            TIFFSetErrorHandler((TIFFErrorHandler)tiffErrorHandler);
            break;
        case LogVerbosity::None:
        default:
            TIFFSetErrorHandler(nullptr);
            TIFFSetWarningHandler(nullptr);
            break;
    }

    // test warning and error logging
    // TIFFError("Test", "libtiff error logging available. %s %i %s", "test", 32, "test");
    // TIFFWarning("Test", "libtiff warning logging available.");
}

}  // namespace inviwo
