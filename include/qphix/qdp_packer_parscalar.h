#ifndef QPHIX_QDP_PACKER_PARSCALAR_H
#define QPHIX_QDP_PACKER_PARSCALAR_H

#warning "building with parscalar packers"

#include "qdp.h"
#include "qphix/geometry.h"

#include "qphix/dslash_def.h"
#include "qphix/qphix_config.h"

#if defined(QPHIX_MIC_SOURCE) || defined(QPHIX_AVX512_SOURCE)
#include <immintrin.h>
#endif

#ifdef QPHIX_BUILD_CLOVER
#include "qphix/clover_dslash_def.h"
#endif

using namespace QDP;

namespace QPhiX { 


  template<typename FT, int veclen, int soalen, bool compress, typename QDPGauge>
    void qdp_pack_gauge(const QDPGauge& u,  
      typename Geometry<FT,veclen,soalen,compress>::SU3MatrixBlock *u_cb0, 
      typename Geometry<FT,veclen,soalen,compress>::SU3MatrixBlock *u_cb1, 
      Geometry<FT,veclen,soalen,compress>& s)
  {
    // Get the subgrid latt size.
    int Nt = s.Nt();
    int Nz = s.Nz();
    int Ny = s.Ny();
    int nvecs = s.nVecs();
    int nyg = s.nGY();
    int Pxy = s.getPxy();
    int Pxyz = s.getPxyz();
    

    // Shift the lattice to get U(x-mu)
    QDPGauge u_minus(4);
    for(int mu=0; mu < 4; mu++) {
      u_minus[mu] = shift(u[mu], BACKWARD, mu);
    }

    
#pragma omp parallel for collapse(4)
    for(int t = 0; t < Nt; t++) {
      for(int z = 0; z < Nz; z++) {
  for(int y = 0; y < Ny; y++) {
    for(int s = 0; s < nvecs; s++) {
      for(int mu = 0; mu < 4; mu++) {
        int outer_c = 3;
        if ( compress ) {
    outer_c = 2;
        }
        for(int c = 0; c < outer_c; c++) {
    for(int c2 = 0; c2 < 3; c2++) {
      for(int x = 0; x < soalen; x++) {

        //#ifndef USE_PACKED_GAUGES
        //int xx = x;
        //int block = ((t*Nz+z)*Ny+y)*nvecs+s;

        //#endif
        //#else // USE_PACKED_GAUGES
        int block = (t*Pxyz+z*Pxy)/nyg+(y/nyg)*nvecs+s;
        int xx = (y%nyg)*soalen+x;
        // #endif // USE_PACKED_GAUGES

        int qdpsite = x + soalen*(s + nvecs*(y + Ny*(z + Nz*t)));
        u_cb0[block][2*mu][c][c2][0][xx] = u_minus[mu].elem(rb[0].start() + qdpsite).elem().elem(c2,c).real();
        u_cb0[block][2*mu][c][c2][1][xx] = u_minus[mu].elem(rb[0].start() + qdpsite).elem().elem(c2,c).imag();
        u_cb0[block][2*mu+1][c][c2][0][xx] = u[mu].elem(rb[0].start() + qdpsite).elem().elem(c2,c).real();
        u_cb0[block][2*mu+1][c][c2][1][xx] = u[mu].elem(rb[0].start() + qdpsite).elem().elem(c2,c).imag();
        
        
        u_cb1[block][2*mu][c][c2][0][xx] = u_minus[mu].elem(rb[1].start() + qdpsite).elem().elem(c2,c).real();
        u_cb1[block][2*mu][c][c2][1][xx] = u_minus[mu].elem(rb[1].start() + qdpsite).elem().elem(c2,c).imag();
        u_cb1[block][2*mu+1][c][c2][0][xx] = u[mu].elem(rb[1].start() + qdpsite).elem().elem(c2,c).real();
        u_cb1[block][2*mu+1][c][c2][1][xx] = u[mu].elem(rb[1].start() + qdpsite).elem().elem(c2,c).imag();
      }   
    }   
        }
      }
    }
  }
      }
    }
  }

#ifdef QPHIX_BUILD_CLOVER
  template<typename FT, int veclen, int soalen, bool compress, typename ClovTerm>
    void qdp_pack_clover(const ClovTerm& qdp_clov_in,
       typename ClovDslash<FT,veclen,soalen,compress>::CloverBlock* cl_out,Geometry<FT,veclen,soalen,compress>& s, int cb)
  {
    // Get the subgrid latt size.
    int Nt = s.Nt();
    int Nz = s.Nz();
    int Ny = s.Ny();
    int nvecs = s.nVecs();
    int nyg = s.nGY();
    int Pxy = s.getPxy();
    int Pxyz = s.getPxyz();
    const auto& qdp_clov_in_buf = qdp_clov_in.getTriBuffer();
 
#pragma omp parallel for collapse(4)
    for(int t = 0; t < Nt; t++) {
      for(int z = 0; z < Nz; z++) {
  for(int y = 0; y < Ny; y++) {
    for(int s = 0; s < nvecs; s++) {
      for(int x = 0; x < soalen; x++) {
        
        int block = (t*Pxyz+z*Pxy)/nyg+(y/nyg)*nvecs+s;
        int xx = (y%nyg)*soalen+x;
        int qdpsite = x + soalen*(s + nvecs*(y + Ny*(z + Nz*t)))+rb[cb].start();
        
        for(int d=0; d < 6; d++) { 
    cl_out[block].diag1[d][xx]=qdp_clov_in_buf[qdpsite].diag[0][d].elem();
        }
        for(int od=0; od < 15; od++) { 
    cl_out[block].off_diag1[od][RE][xx]=qdp_clov_in_buf[qdpsite].offd[0][od].real();
    cl_out[block].off_diag1[od][IM][xx]=qdp_clov_in_buf[qdpsite].offd[0][od].imag();
        }

        for(int d=0; d < 6; d++) { 
    cl_out[block].diag2[d][xx]=qdp_clov_in_buf[qdpsite].diag[1][d].elem();
        }
        for(int od=0; od < 15; od++) { 
    cl_out[block].off_diag2[od][RE][xx]=qdp_clov_in_buf[qdpsite].offd[1][od].real();
    cl_out[block].off_diag2[od][IM][xx]=qdp_clov_in_buf[qdpsite].offd[1][od].imag();
        }
      }
    }
  }
      }
    }
  }
  
#endif  // IFDEF BUILD CLOVER


  template<typename FT, int veclen, int soalen, bool compress, typename QDPSpinor>
  void qdp_pack_cb_spinor(const QDPSpinor& psi_in, 
        typename Geometry<FT,veclen,soalen, compress>::FourSpinorBlock* psi,
        Geometry<FT,veclen,soalen,compress>& s,
        int cb) 
  { 
    // Get the subgrid latt size.
    int Nt = s.Nt();
    int Nz = s.Nz();
    int Ny = s.Ny();
    int Nxh = s.Nxh();
    int nvecs = s.nVecs();
    int Pxy = s.getPxy();
    int Pxyz = s.getPxyz();

#pragma omp parallel for collapse(4)
      for(int t=0; t < Nt; t++) {
  for(int z=0; z < Nz; z++) {
    for(int y=0; y < Ny; y++) {
      for(int s=0; s < nvecs; s++) { 
        for(int col=0; col < 3; col++)  {
    for(int spin=0; spin < 4; spin++) { 
      for(int x=0; x < soalen; x++) { 

        int ind = t*Pxyz+z*Pxy+y*nvecs+s; //((t*Nz+z)*Ny+y)*nvecs+s;
        int x_coord = s*soalen + x;
        
        int qdp_ind = ((t*Nz + z)*Ny + y)*Nxh + x_coord;
        psi[ind][col][spin][0][x] = psi_in.elem(rb[cb].start()+qdp_ind).elem(spin).elem(col).real();
        psi[ind][col][spin][1][x] = psi_in.elem(rb[cb].start()+qdp_ind).elem(spin).elem(col).imag();
      
      }
    }
        }
      }
    }
  }
      }
      
  }


  template<typename FT, int veclen, int soalen, bool compress, typename QDPSpinor>
    void qdp_unpack_cb_spinor(typename Geometry<FT,veclen,soalen,compress>::FourSpinorBlock* chi_packed, 
            QDPSpinor& chi,
            Geometry<FT,veclen,soalen,compress>& s,
            int cb) 
  { 
    int Nt = s.Nt();
    int Nz = s.Nz();
    int Ny = s.Ny();
    int Nxh = s.Nxh();
    int nvecs = s.nVecs();
    int Pxy = s.getPxy();
    int Pxyz = s.getPxyz();


#pragma omp parallel for collapse(4)    
    for(int t=0; t < Nt; t++) {
      for(int z=0; z < Nz; z++) {
  for(int y=0; y < Ny; y++) {
    for(int s=0; s < nvecs; s++) { 
      for(int spin=0; spin < 4; spin++) { 
        for(int col=0; col < 3; col++)  {
    for(int x=0; x < soalen; x++) { 

      int ind = t*Pxyz+z*Pxy+y*nvecs+s; //((t*Nz+z)*Ny+y)*nvecs+s;
      int x_coord = s*soalen + x;
      
      int qdp_ind = ((t*Nz + z)*Ny + y)*Nxh + x_coord;
      
      chi.elem(rb[cb].start()+qdp_ind).elem(spin).elem(col).real() =  chi_packed[ind][col][spin][0][x];
      chi.elem(rb[cb].start()+qdp_ind).elem(spin).elem(col).imag() =  chi_packed[ind][col][spin][1][x];

    }
        }
      }
    }
  }
      }
    }
  }

};


#endif