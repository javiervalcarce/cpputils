#ifndef PTSKERNEL_DSP_FIR_H_
#define PTSKERNEL_DSP_FIR_H_


class Fir {

 public:

      /**
       * h es la respuesta al impulso del filtro
       * n es el numero de coeficientes de h, el orden del filtro es (n - 1)
       */
      Fir (float* h, int n);

      ~Fir();

      // Operacion de filtrado
      void Filter(float* x, float* y, int sz);

      // Calculo de los coeficiente de un filtro paso banda de tipo Butterworth
      void Butterworth(float w0, float w1, float* coeff, int n);
      


 private:
      int n_;
      int order_;
      int c_;       // current

      float* h_;
      float* d_;
};

#endif  // PTSKERNEL_DSP_FIR_H_
