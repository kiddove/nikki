#pragma once
//#pragma pack(4)

struct GLA14
{
public:
	int i_rec_ndx;
	int i_UTCTime[2];      //2
	short i_transtime;
	byte i_Spare1[2];        //2
	int i_deltagpstmcor;
	int i_dShortTime[39];   //39
	int i_lat[40];          //40
	int i_lon[40];          //40
	int i_elev[40];         //40
	byte i_campaign[2];      //2
	short i_spare40;
	int i_cycTrk;
	int i_localSolarTime;
	int i_spare41[7];      //7
	short i_deltaEllip[40];   //40
	int i_beamCoelv[40];    //40
	int i_beamAzimuth[40];	//40
	int i_d2refTrk[40];     //40
	int i_SigBegOff[40];    //40
	byte i_DEM_hires_src[40]; //40
	short i_DEMhiresArelv[9][40];   //9X40
	short i_ElevBiasCorr[40]; //40
	short i_spare42[4][40];     //4X40
	short i_sigmaatt[40];    //40
	int i_Azimuth;
	int i_AolAng;
	int i_tpintensity_avg;
	short i_tpazimuth_avg;
	short i_tpeccentricity_avg;
	short i_tpmajoraxis_avg;
	byte i_poleTide[2];      //2
	short i_gdHt[2];         //2
	short i_erElv[2];        //2
	short i_spElv[4];        //4
	short i_ldElv[4];        //4
	short i_spare12[2];      //2
	short i_wTrop[2];        //2
	short i_dTrop[40];        //40
	byte i_surfType;
	byte i_spare11[3];       //3
	int i_DEM_elv[40];      //40
	int i_refRng[40];       //40
	int i_spare47[40];      //40
	int i_ldRngOff[40];     //40
	int i_SigEndOff[40];    //40
	int i_gpCntRngOff[6][40]; //6X40
	int i_reflctUC[40];     //40
	int i_reflCor_atm;
	short i_maxSmAmp[40];     //40
	short i_ocElv[40];        //40
	byte i_numPk[40];         //40
	short i_kurt1[40];        //40
	short i_skew1[40];        //40
	byte i_spare4[160];        //160
	int i_Gamp[6][40];        //6X40
	int i_Garea[6][40];       //6X40
	int i_gsinma[6][40];      //6X40
	byte i_nPeaks1[40];       //40
	short i_LandVar[40];      //40
	byte i_ElvuseFlg[5];     //5
	byte i_atm_avail;
	byte i_spare16[4];       //4
	byte i_cld1_mswf;
	byte i_MRC_af;
	byte i_spare9[40];        //40
	byte i_ElvFlg[40];        //40
	short i_rng_UQF[40];      //40
	byte i_spare49[10];       //10
	short i_timecorflg;
	byte i_APID_AvFlg[8];    //8
	byte i_AttFlg2[20];       //20
	byte i_spare5;
	byte i_FrameQF;
	byte i_OrbFlg[2];        //2
	byte i_rngCorrFlg[2];    //2
	byte i_CorrStatFlg[2];   //2
	byte i_spare15[8];       //8
	short i_Attflg1;
	byte i_Spare6[2];        //2
	byte i_spare44[120];       //120
	byte i_satNdx[40];        //40
	short i_satElevCorr[40];  //40
	byte i_satCorrFlg[40];    //40
	short i_satNrgCorr[40];   //40
	short i_spare13[40];      //40
	short i_gval_rcv[40];     //40
	short i_RecNrgAll[40];    //40
	short i_FRir_cldtop[40];  //40
	byte i_FRir_qaFlag[40];   //40
	short i_atm_char_flag;
	short i_atm_char_conf;
	byte i_spare48[36];       //36
	short i_FRir_intsig[40];  //40
	byte i_spare14[120];       //120
	short i_Surface_temp;
	short i_Surface_pres;
	short i_Surface_relh;
	short i_maxRecAmp[40];    //40
	short i_sDevNsOb1[40];    //40
	byte i_spare8[2];        //2
	int i_isRngOff[40];     //40
	byte i_pctSAT[40];        //40
	short i_TxNrg[40];        //40
	short i_eqElv[2];        //2
	byte i_spare7[120];        //120
};

struct GLA01_MAIN
{
	int i_rec_ndx;
	int i_UTCTime[2];
	short i_gla01_rectype;
	short i_spare1;
	int i_dShotTime[39];
	int i1_pred_lat;
	int i1_pred_lon;
	int i_RespEndTime[40];
	int i_LastThrXingT[40];
	int i_NextThrXingT[40];
	int iEchoPeakLoc[40];
	short i_EchoPeakVal[40];
	int i_wt_fact_filt[6][40];
	short i_filtr_thresh[40];
	int i_time_txWfPk[40];
	int i_TxWfStart[40];
	int i_TxNrg_Eu;
	int i_RecNrgAll_EU[40];
	int i_RecNrgLast_EU[40];
	byte i_txWfPk_Flag[40];
	int i_lnstState;
	byte i_APID_AvFlg[8];
	int i_FitNumMask;
	int i_HOff[2];
	int i_ADBias[2];
	int i_RminRmax[2];
	int i_WMinMax[2];
	int i_ObSCHt;
	short i_engineering[12];
	short i_compRatio[2];
	short i_N_val;
	short i_r_val;
	short i_ADdetOutGn;
	short i_DEMmin;
	short i_DEMmax;
	byte i_tx_wf[48][40];
	byte i_OrbFlg[2];
	byte i_EchoLandType;
	byte i_RngSrc_Flag;
	short i_timecorflg;
	byte i_TxFlg[5];
	byte i_GainShiftFlg[5];
	byte i_spare2[10];
};

struct GLA01_LONG
{
	int i_rec_ndx;
	int i_UTCTime[2];
	short i_gla01_rectype;
	short i_spare1;
	byte i_filtnum[8];
	short i_shot_ctr[8];
	int i_statflags[8];
	short i_gainSet1064[8];
	short i_4nsPeakVal[8];
	short i_8nsPeakVal[8];
	unsigned short i_4nsBgMean[8];
	unsigned short i_4nsBgSDEV[8];
	short i_samp_pad[8];
	byte i_comp_type[8];
	byte i_rng_wf[544][8];
	byte i_gainStatus[8];
	byte i_NumCoinc[8];
	byte i_rawPkHt[8];
	byte i_spare2[108];
};

struct GLA01_SHORT
{
	int i_rec_ndx;
	int i_UTCTime[2];
	short i_gla01_rectype;
	short i_spare1;
	byte i_filtnum[20];
	short i_shot_ctr[20];
	int istatflags[20];
	short i_gainSet1064[20];
	short i_4nsPeakVal[20];
	short i_8nsPeakVal[20];
	unsigned short i_4nsBgMead[20];
	unsigned short i_4nsBgSDEV[20];
	short i_samp_pad[20];
	byte i_comp_type[20];
	byte i_rng_wf[200][20];
	byte i_gainStatus[20];
	byte i_NumCoinc[20];
	byte i_rawPkHt[20];
	byte i_spare2[184];
};
//#pragma pack()