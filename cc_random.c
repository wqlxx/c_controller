/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
 * Junior University
 *
 * We are making the OpenFlow specification and associated documentation
 * (Software) available for public use and benefit with the expectation
 * that others will use, modify and enhance the Software and contribute
 * those enhancements back to the community. However, since we would
 * like to make the Software available for broadest use, with as few
 * restrictions as possible permission is hereby granted, free of
 * charge, to any person obtaining a copy of this Software to deal in
 * the Software under the copyrights without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */
#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>

#include "random.h"

void
cc_random_init(void)
{
    static bool inited = false;
    if (!inited) {
        struct timeval tv;
        inited = true;
        if (gettimeofday(&tv, NULL) < 0) {
            return;    
        }
        srand(tv.tv_sec ^ tv.tv_usec);
    }
}

void
cc_random_bytes(void *p_, size_t n)
{
    uint8_t *p = p_;
    cc_random_init();
    while (n--) {
        *p++ = rand();
    }
}

uint8_t
cc_random_uint8(void)
{
    cc_random_init();
    return rand();
}

uint16_t
cc_random_uint16(void)
{
    if (RAND_MAX >= UINT16_MAX) {
        cc_random_init();
        return rand();
    } else {
        uint16_t x;
        cc_random_bytes(&x, sizeof x);
        return x;
    }
}

uint32_t
cc_random_uint32(void)
{
    if (RAND_MAX >= UINT32_MAX) {
        cc_random_init();
        return rand();
    } else if (RAND_MAX == INT32_MAX) {
        cc_random_init();
        return rand() | ((rand() & 1u) << 31);
    } else {
        uint32_t x;
        cc_random_bytes(&x, sizeof x);
        return x;
    }
}

int
cc_random_range(int max) 
{
    return cc_random_uint32() % max;
}

static inline uint32_t
cc_create_xid(void)
{
    return cc_random_uint32();
}


