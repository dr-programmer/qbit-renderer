#include <iostream>
#include <fstream>

#include <cmath>

extern "C" {

#define SMART_DEALLOCATION
#include "qbit.h"

}

static const int imageWidth = 1920;
static const int imageHeight = 1080;
static const int maxColorComponent = 255;

struct matrix *convert_2dc_3dr(vector *v) {
    if(!v) return NULL;

    float y = pow(complex_get_absolute(v->fields[0][0]), 2) 
                    - pow(complex_get_absolute(v->fields[1][0]), 2);
    float x_sign = complex_get_absolute(v->fields[1][0]) != 0 
                    ? (v->fields[1][0].real 
                        / complex_get_absolute(v->fields[1][0])) 
                    : 0;
    float sin_phi = (pow(complex_get_absolute(v->fields[0][0]), 2) 
                        + pow(complex_get_absolute(v->fields[1][0]), 2)) 
                    * x_sign;
    float x = sqrt(1 - pow(y, 2)) * sin_phi;
    float z_sign = complex_get_absolute(v->fields[1][0]) != 0 
                    ? (v->fields[1][0].imaginary 
                        / complex_get_absolute(v->fields[1][0])) 
                    : 0;
    float z = 1 - pow(sin_phi, 2) == 1 
                ? sqrt(1 - pow(y, 2)) * sqrt(1 - pow(sin_phi, 2)) * z_sign 
                : 0;

    // float x, y, z;
    // S
    // quantum_operator *d = matrix_mul(v, vector_get_dual(v));
    // x = -2 * d->fields[0][0].real;
    // z = 2 * d->fields[1][0].imaginary;
    // y = complex_mul(complex_create(2, 0), d->fields[0][0]).real - 1;
    // E

    struct matrix *result = matrix_create_empty(3, 1);

    result->fields[0][0].real = x;
    result->fields[1][0].real = y;
    result->fields[2][0].real = z;

    return result;
}

vector *vector_cross_product(const vector * const v1, const vector * const v2) {
    if(!v1 || !v2) return NULL;

    float sqrt2 = sqrt(2);

    float ax = v1->fields[1][0].real * sqrt2;
    float ay = v1->fields[0][0].real - v1->fields[1][0].real - v1->fields[1][0].imaginary;
    float az = v1->fields[1][0].imaginary * sqrt2;

    float bx = v2->fields[1][0].real * sqrt2;
    float by = v2->fields[0][0].real - v2->fields[1][0].real - v2->fields[1][0].imaginary;
    float bz = v2->fields[1][0].imaginary * sqrt2;

    return vector_create_init(
        complex_create(fabs((ax*by+ay*bx+ay*bz+az*by)/sqrt2 + az*bx), 0), 
        complex_create((ay*bz-az*by)/sqrt2 + ax*bz, (ax*by-ay*bx)/sqrt2)
    );
}

int main() {

smart_allocation_global_turn_off = 1;

S
    S
    struct complex c_0 = complex_create(0, 0);
    struct complex c_1 = complex_create(1, 0);
    qubit *points[3] = {
        qubit_create_init(complex_create(1/sqrt(2), 0), complex_create(-1/sqrt(2), -1/sqrt(2))), 
        qubit_create_init(complex_create(1/sqrt(2), 0), complex_create(0, -1/sqrt(2))), 
        qubit_create_init(c_1, complex_create(0, -1/sqrt(2)))
    };
    for(unsigned int i = 0; i < 3; i++) P(points[i]);

    matrix_print(points[0]);
    printf("x\n");
    matrix_print(points[1]);
    printf("=\n");
    vector *test_cross = vector_cross_product(points[0], points[1]);
    matrix_print(test_cross);
    printf("=>\n");
    struct matrix *test = convert_2dc_3dr(test_cross);
    matrix_print(test);

    E

    for(unsigned int i = 0; i < 3; i++) {
        S
        printf("\n");
        matrix_print(points[i]);

        struct matrix *coords_3d = convert_2dc_3dr(points[i]);

        printf("x: %f, y: %f, z: %f \n", 
                    coords_3d->fields[0][0].real, 
                    coords_3d->fields[1][0].real, 
                    coords_3d->fields[2][0].real);
        E
    }

    matrix_print(matrix_sub(convert_2dc_3dr(points[0]), convert_2dc_3dr(points[1])));
    matrix_print(matrix_sub(points[0], points[1]));
    matrix_print(convert_2dc_3dr(matrix_add(points[0], points[1])));

    vector *normal = vector_create_init(complex_create(1/sqrt(2), 0), complex_create(0, 1/sqrt(2)));

    std::ofstream ppmFileStream("test.ppm", 
                    std::ios::out | std::ios::binary);
    ppmFileStream << "P3\n";
    ppmFileStream << imageWidth << " " << imageHeight << "\n";
    ppmFileStream << maxColorComponent << "\n";
    for (int rowIdx = 0; rowIdx < imageHeight; ++rowIdx) {
        for (int colIdx = 0; colIdx < imageWidth; ++colIdx) {
            float x = colIdx + 0.5, y = rowIdx + 0.5;
            x /= imageWidth;
            y /= imageHeight;
            x = (2 * x) - 1;
            y = 1 - (2 * y);
            x *= imageWidth / imageHeight;          

            S
            qubit *qm = NULL;
            // |p> = cos(th/2)|0> + sin(th/2)e^(i*gam)|1>
            float theta = acos(y);
            float phi = atan2(0, x);
            qm = matrix_add(
                matrix_mul_scalar(complex_create(cos(theta/2), 0), qubit_create()), 
                matrix_mul_scalar(
                    complex_mul(complex_create(sin(theta/2), 0), complex_create(cos(phi), sin(phi))), 
                    qubit_create_init(complex_create(0, 0), complex_create(0, 1))
                )
            );

            // float length = vector_inner_product(qm, qm).real;
            // qm->fields[0][0].real /= length;
            // qm->fields[1][0].real /= length;

            // float scale_factor = fabs(
            //     vector_inner_product(points[0], normal).real 
            //         / vector_inner_product(qm, normal).real
            // );

            // qm->fields[0][0].real *= scale_factor;
            // qm->fields[1][0].real *= scale_factor;
            // qm->fields[1][0].imaginary *= scale_factor;

            // qm checking
            // vector *conv = convert_2dc_3dr(qm);
            // matrix_print(qm);
            // printf("x: %f, y: %f, z: %f \n", 
            //         conv->fields[0][0].real, 
            //         conv->fields[1][0].real, 
            //         conv->fields[2][0].real);

            vector *x_check = matrix_mul(quantum_gate_create_hadamard(), qm);

            bool isIn = true;
            // if(pow(qm->fields[0][0].real, 2) - pow(qm->fields[1][0].real, 2) 
            //         < points[0]->fields[0][0].real 
            //     && pow(x_check->fields[0][0].real, 2) - pow(x_check->fields[1][0].real, 2) 
            //         < points[0]->fields[0][0].real) isIn = false;

            S
            for(unsigned int i = 0; i < 3; i++) {
                unsigned int tail = i;
                unsigned int head = (i + 1) % 3;
                vector *temp = vector_cross_product(
                    matrix_sub(points[head], points[tail]), 
                    matrix_sub(qm, points[tail])
                );

                // vector *conv = convert_2dc_3dr(temp);
                // matrix_print(temp);
                // printf("x: %f, y: %f, z: %f \n", 
                //         conv->fields[0][0].real, 
                //         conv->fields[1][0].real, 
                //         conv->fields[2][0].real);

                struct complex checkSum = vector_inner_product(temp, normal);

                if(checkSum.real < 0) {
                    isIn = false;
                    break;
                }
            }
            E

            int x_color = 0;
            int z_color = 0;

            if(isIn) {
                // x_color = (int)abs(qm->fields[0][0].real * 1000) % maxColorComponent;
                // z_color = (int)abs(qm->fields[1][0].real * 1000) % maxColorComponent;
                x_color = 100;
                z_color = 100;
            }

            ppmFileStream << x_color
            << " " << 100 % maxColorComponent
            << " " << z_color
            << "\t";
            E
        }
        ppmFileStream << "\n";
    }
    ppmFileStream.close();
E
    return 0;
}