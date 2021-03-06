/*
 *  Copyright (C) 2010 by Tim Massingham, European Bioinformatics Institute
 *  tim.massingham@ebi.ac.uk
 *
 *  This file is part of the simNGS software for simulating likelihoods
 *  for next-generation sequencing machines.
 *
 *  simNGS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  simNGS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with simNGS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <tgmath.h>
#include <stdbool.h>
#include <assert.h>
#include "random.h"
#include "utility.h"

/* Cumulative density function of Weibull distribution */
/* Four possible cases depending on tail or log, deal with
 * all of them accurately
 */
real_t pweibull (  real_t x,  real_t shape,  real_t scale,  bool tail,  bool logp){
    validate(x>0,NAN);
    validate(shape>0,NAN);
    validate(scale>0,NAN);
        
    real_t res = -pow(x/scale,shape);
    if( (true==tail) && (true==logp) ){ return res; }
    
    if ( (false==tail) && (false==logp) ){ return -expm1(res); } 
    
    res = exp(res);
    if ( true==tail ){ return res;}
    
    return log1p(-res);
}

/* Inverse CDF of Weibull, dealing with log and tails correctly */
real_t qweibull ( real_t p,  real_t shape,  real_t scale,  bool tail,  bool logp){
    validate(shape>0,NAN);
    validate(shape>0,NAN);
    validate(p>=0 && p<=1,NAN);
    
    // Default cases
    if(NAN==p){ return NAN; }
    if(p<=0.){ return 0.;}
    if(p>=1.){ return HUGE_VAL;}
    
    real_t res = (true==tail) ? -log(p) : -log1p(-p);
    
    if ( false==logp ){ return scale*pow(res,1.0/shape); }
    
    return log(scale) + log(res)/shape;
}

/* Weibull density */
real_t dweibull (  real_t x, real_t shape,  real_t scale,  bool logd ){
    validate(x>=0.0,NAN);
    validate(shape>0,NAN);
    validate(shape>0,NAN);

    if ( NAN==x){ return NAN;}
    real_t scaledx = x/scale;
    if(true==logd){ return log(shape/scale) + (shape-1) * log(scaledx) - pow(scaledx,shape);}
    
    real_t res = pow(scaledx,shape-1.);
    return (shape/scale) * res * exp( -res*scaledx );
}

/* Random Weibull */
/* Sample by inverse CDF since this is analytic */
real_t rweibull(  real_t shape,  real_t scale ){
    validate(shape>0,NAN);
    validate(shape>0,NAN);
    /* Uses property that U uniform implies 1-U uniform
     * on the assumption that log(p) will be faster that log1p
     * on most platforms (more effort spent optimising). See
     * qweibull.
     */
    return qweibull(runif(),shape,scale,true,false);
}

#ifdef TEST
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

bool hassuffix( const char * str, const char * suf ){
    const uint32_t suf_len = strlen(suf);
    const uint32_t str_len = strlen(str);
    if ( suf_len>str_len){ return false;}
    return (0==strcmp(str+(str_len-suf_len),suf));
}
    
int main ( int argc, char * argv[] ){
    if(1==argc){
        fputs("Usage:\n"
"qweibull x shape scale tail logp\n"
"pweibull p shape scale tail logp\n"
"dweibull x shape scale log\n"
"rweibull shape scale seed\n", stderr);
        return EXIT_FAILURE;
    }
    
    double x,p,shape,scale;
    unsigned int tail,logp;
    long unsigned int seed;
    printf("%s",argv[0]);

    if ( hassuffix(argv[0],"qweibull") ){
        sscanf(argv[1],"%le",&x);
        sscanf(argv[2],"%le",&shape);
        sscanf(argv[3],"%le",&scale);
        sscanf(argv[4],"%u",&tail);
        sscanf(argv[5],"%u",&logp);
        printf("(%e,%e,%e,%u,%u)=",x,shape,scale,tail,logp);
        printf("%e\n",qweibull(x,shape,scale,tail,logp));
        return EXIT_SUCCESS;
    }

    if ( hassuffix(argv[0],"pweibull") ){
        sscanf(argv[1],"%le",&p);
        sscanf(argv[2],"%le",&shape);
        sscanf(argv[3],"%le",&scale);
        sscanf(argv[4],"%u",&tail);
        sscanf(argv[5],"%u",&logp);
        printf("(%e,%e,%e,%u,%u)=",p,shape,scale,tail,logp);
        printf("%e\n",pweibull(p,shape,scale,tail,logp));
        return EXIT_SUCCESS;
    }
    
    if ( hassuffix(argv[0],"dweibull") ){
        sscanf(argv[1],"%le",&x);
        sscanf(argv[2],"%le",&shape);
        sscanf(argv[3],"%le",&scale);
        sscanf(argv[4],"%u",&logp);
        printf("(%e,%e,%e,%u)=",x,shape,scale,logp);
        printf("%e\n",dweibull(x,shape,scale,logp));
        return EXIT_SUCCESS;
    }
    
    if ( hassuffix(argv[0],"rweibull") ){
        sscanf(argv[1],"%le",&shape);
        sscanf(argv[2],"%le",&scale);
        sscanf(argv[3],"%lu",&seed);
        init_gen_rand(seed);
        printf("(%e,%e,%lu)=",shape,scale,seed);
        printf("%e\n",rweibull(shape,scale));
        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}
#endif

