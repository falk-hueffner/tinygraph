/* tinygraph -- exploring graph conjectures on small graphs
   Copyright (C) 2015  Falk Hüffner

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  */

#include "EulerTransform.hh"

#include "catch.hh"

std::vector<EulerTransform::bignum> numConnectedGraphs = {
    1_mpz, 1_mpz, 1_mpz, 2_mpz, 6_mpz, 21_mpz, 112_mpz, 853_mpz, 11117_mpz, 261080_mpz, 11716571_mpz, 1006700565_mpz, 164059830476_mpz, 50335907869219_mpz, 29003487462848061_mpz, 31397381142761241960_mpz, 63969560113225176176277_mpz, 245871831682084026519528568_mpz, 1787331725248899088890200576580_mpz, 24636021429399867655322650759681644_mpz,
};

std::vector<EulerTransform::bignum> numGraphs = {
    1_mpz, 1_mpz, 2_mpz, 4_mpz, 11_mpz, 34_mpz, 156_mpz, 1044_mpz, 12346_mpz, 274668_mpz, 12005168_mpz, 1018997864_mpz, 165091172592_mpz, 50502031367952_mpz, 29054155657235488_mpz, 31426485969804308768_mpz, 64001015704527557894928_mpz, 245935864153532932683719776_mpz, 1787577725145611700547878190848_mpz, 24637809253125004524383007491432768_mpz,
};

std::vector<uint64_t> numConnectedTriangleFreeGraphs = {
    1, 1, 1, 1, 3, 6, 19, 59, 267, 1380, 9832, 90842, 1144061, 19425052, 445781050, 13743625184, 566756900370, 31125101479652
};

std::vector<EulerTransform::bignum> numTriangleFreeGraphs = {
    1_mpz, 1_mpz, 2_mpz, 3_mpz, 7_mpz, 14_mpz, 38_mpz, 107_mpz, 410_mpz, 1897_mpz, 12172_mpz, 105071_mpz, 1262180_mpz, 20797002_mpz, 467871369_mpz, 14232552452_mpz, 581460254001_mpz, 31720840164950_mpz,
};

TEST_CASE("transform", "[EulerTransform]" ) {
    REQUIRE(EulerTransform::transform(numConnectedGraphs) == numGraphs);
    REQUIRE(EulerTransform::transform(numConnectedTriangleFreeGraphs) == numTriangleFreeGraphs);
}
