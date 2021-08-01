/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	
 * Module:
 * Project:	lwdfwiz
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2021 Robinson Mittmann. All Rights Reserved.
 *
 * LWDFWiz is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <complex.h>


#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------
 * Single precision floating point 
 * ---------------------------------------------------------------------------
 * */

ssize_t vec_fp32_cosine(float y[], size_t len, float w0);

ssize_t vec_fp32_wnd_blackman(float y[], size_t len, float alpha);

complex float vec_fp32_gortzel_dft(const float x[], size_t len, float w);

/* ---------------------------------------------------------------------------
 * Double precision floating point 
 * ---------------------------------------------------------------------------
 * */

ssize_t vec_fp64_cosine(double y[], size_t len, double w0);

ssize_t vec_fp64_wnd_blackman(double y[], size_t len, double alpha);

complex double vec_fp64_gortzel_dft(const double x[], size_t len, double w);


#ifdef __cplusplus
}
#endif
#endif /* __VECTOR_H__ */

