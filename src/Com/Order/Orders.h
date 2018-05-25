//
// Created by asphox on 29/04/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_ORDERS_H
#define TECHTHETOWN_LOWLEVEL_ORDERS_H

#include "AbstractOrder.h"
#include "OrderManager.h"
#include "MotionControl/trajectories.h"

ORDER(UNKOWN,0);

ORDER(PING,0);
ORDER(J,0);
ORDER(F,0);
ORDER(XYO,0);
ORDER(D,1);
ORDER(T,1);
ORDER(GOTO,2);
ORDER(FOLLOWTRAJECTORY,1);
ORDER(STOP,0);
ORDER(EMERGENCYSTOP,0);
ORDER(RESUMEEMERGENCYSTOP,0);

/*			 __________________
* 		   *|                  |*
*		   *|   POS & VITESSE  |*
*		   *|__________________|*
*/

ORDER(CX,1);
ORDER(CY,1);
ORDER(CO,1);
ORDER(CXYO,3);
ORDER(CTV,1);
ORDER(CRV,1);
ORDER(CTRV,2);
ORDER(EFM,0);
ORDER(DFM,0);

/*			 ___________________
* 		   *|                   |*
*		   *|  ASSERVISSEMENTS  |*
*		   *|___________________|*
*/

ORDER(CT0,0);
ORDER(CT1,0);
ORDER(CR0,0);
ORDER(CR1,0);
ORDER(CV0,0);
ORDER(CV1,0);

/*			 _________________________________
* 		   *|                                 |*
*		   *|			  Capteurs            |*
*    	   *|_________________________________|*
*/

/*			 ___________________________
* 		   *|                           |*
*		   *|					        |*
*		   *|			DEBUG			|*
*		   *|						    |*
*		   *|___________________________|*
*/

ORDER(COD,0);
ORDER(PFDEBUG,0);
ORDER(RAWPWM,1);
ORDER(GETPWM,0);
ORDER(ERRORS,0);
ORDER(RAWSPEED,0);
ORDER(RAWPOSDATA,0);

/*			 ___________________________
* 		   *|                           |*
*		   *|         MONTLHERY         |*
*		   *|   DEPLACEMENT ET ROTATION |*
*		   *|    SANS ASSERVISSEMENT    |*
*		   *|___________________________|*
*/

ORDER(MONTLHERY,0);
ORDER(AV,0);
ORDER(RC,0);
ORDER(TD,0);
ORDER(TG,0);
ORDER(SSTOP,0);

/*			 _________________________________
* 		   *|                                 |*
*		   *|CONSTANTES D'ASSERV (pour le PID)|*
*    	   *|_________________________________|*
*/

ORDER(TOGGLE,0);
ORDER(DISPLAY,0);
ORDER(KPT,1);
ORDER(KDT,1);
ORDER(KIT,1);
ORDER(KPR,1);
ORDER(KIR,1);
ORDER(KDR,1);
ORDER(KPG,1);
ORDER(KIG,1);
ORDER(KDG,1);
ORDER(KPD,1);
ORDER(KID,1);
ORDER(KDD,1);

/*			 _________________________________
* 		   *|                                 |*
*		   *|			 Actionneurs          |*
*    	   *|_________________________________|*
*/

ORDER(AXM,2);
ORDER(AXGM,2);
ORDER(AXS,2);
ORDER(AXGS,2);

/*			 _________________________________
 * 		   *|                                 |*
 *		   *|     Actionneurs spécifiques     |*
 *    	   *|_________________________________|*
 */

/*
 * BRAS
 */
ORDER(BLBAVBEI,0);
ORDER(BLBAV,0);
ORDER(RLBAV,0);
ORDER(BLBARBEI,0);
ORDER(BLBAR,0);
ORDER(RLBAR,0);
/*
 * PORTES
 */
ORDER(FLPAV,0);
ORDER(OLPAV,0);
ORDER(FLPAR,0);
ORDER(OLPAR,0);
ORDER(OLPAVP,0);
ORDER(OLPARP,0);
/*
 * POMPE
 */
ORDER(ALP,0);
ORDER(DLP,0);
/*
 * ELECTROVANNES
 */
ORDER(AEAV,0);
ORDER(DEAV,0);
ORDER(AEAR,0);
ORDER(DEAR,0);

/*			 _________________________________
* 		   *|                                 |*
*		   *|			  Capteurs            |*
*    	   *|_________________________________|*
*/

ORDER(SUS,1);
ORDER(CCAV,0);
ORDER(CCAR,0);
ORDER(BDE,0);
ORDER(BDD,0);

/*			 _________________________________
* 		   *|                                 |*
*		   *|			   HOOKS	          |*
*    	   *|_________________________________|*
*/

ORDER(NH,8);
ORDER(EH,1);
ORDER(DH,1);

/*			 _________________________________
* 		   *|                                 |*
 *		   *|	       ACKNOWLEDGEMENT        |*
 *    	   *|_________________________________|*
*/

ORDER(ACK,1);
ORDER(DEMO,0);
ORDER(PTPDEMO,0);
ORDER(PTPDEMOSEQ,0);
#endif //TECHTHETOWN_LOWLEVEL_ORDERS_H
