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

int main() {

smart_allocation_global_turn_off = 1;

S
    S
    qubit *points[3] = {
        qubit_create_init(complex_create(1, 0), complex_create(0, 0)), 
        qubit_create_init(complex_create(0, 0), complex_create(0, 0)), 
        matrix_mul(quantum_gate_create_hadamard(), qubit_create())
    };
    for(unsigned int i = 0; i < 3; i++) P(points[i]);
    E

    for(unsigned int i = 0; i < 3; i++) {
        matrix_print(points[i]);
        printf("\n");
    }

    vector *normal = vector_create_init(complex_create(1/sqrt(2), 0), complex_create(0, 1/sqrt(2)));
    quantum_gate *o = quantum_gate_create(2);
    o->fields[1][1] = complex_create(0, 1);

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
            qm = matrix_add(
                matrix_mul_scalar(complex_create(x, 0), qubit_create()), 
                matrix_mul_scalar(
                    complex_create(y, 0), 
                    qubit_create_init(complex_create(0, 0), complex_create(0, 1))
                )
            );

            float length = vector_inner_product(qm, qm).real;
            qm->fields[0][0].real /= length;
            qm->fields[1][0].real /= length;

            float scale_factor = fabs(
                vector_inner_product(points[0], normal).real 
                    / vector_inner_product(qm, normal).real
            );

            qm->fields[0][0].real *= scale_factor;
            qm->fields[1][0].real *= scale_factor;

            vector *x_check = matrix_mul(quantum_gate_create_hadamard(), qm);

            bool isIn = true;
            for(unsigned int i = 0; i < 3; i++) {
                unsigned int tail = i;
                unsigned int head = (i + 1) % 3;
                S
                vector *arm = matrix_sub(points[head], points[tail]);
                vector *dir = matrix_sub(qm, points[tail]);
                vector *temp = matrix_mul(o, arm);
                struct complex check = vector_inner_product(normal, temp);
                if(check.real < 0) {
                    isIn = false;
                    E
                    break;
                }
                E
            }

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