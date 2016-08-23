#ifndef CLUSTER_HH
#define CLUSTER_HH

struct Cluster
{
  float pt                  ;
  float eta                 ;
  float phi                 ;
  float dphi_jet            ;
  float e                   ;
  unsigned int clustersize  ;
  float isolation           ;
  float lateral             ;
  float longitudinal        ;
  float second_lambda       ;
  float second_r            ;
  float center_lambda       ;
  float center_mag          ;
  float eng_pos             ;
  float em_probability      ;
  float eng_frac_max        ;
  float first_eng_dens      ;
};


#endif
