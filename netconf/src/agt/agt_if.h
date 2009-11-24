/*
 * Copyright (c) 2009, Netconf Central, Inc.
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.    
 */
#ifndef _H_agt_if
#define _H_agt_if
/*  FILE: agt_if.h
*********************************************************************
*                                                                   *
*                         P U R P O S E                             *
*                                                                   *
*********************************************************************

   NETCONF interface entry monitoring

*********************************************************************
*                                                                   *
*                   C H A N G E         H I S T O R Y               *
*                                                                   *
*********************************************************************

date             init     comment
----------------------------------------------------------------------
18-jul-09    abb      Begun.
*/

#ifndef _H_status
#include "status.h"
#endif

/********************************************************************
*                                                                   *
*                         C O N S T A N T S                         *
*                                                                   *
*********************************************************************/

/********************************************************************
*                                                                   *
*                             T Y P E S                             *
*                                                                   *
*********************************************************************/


/********************************************************************
*                                                                   *
*                        F U N C T I O N S                          *
*                                                                   *
*********************************************************************/

extern status_t
    agt_if_init (void);

extern status_t
    agt_if_init2 (void);

extern void 
    agt_if_cleanup (void);

#endif            /* _H_agt_if */
