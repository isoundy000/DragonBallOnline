#include "precomp_trigger.h"
#include "DboTSQCtrl.h"
#include "DboTSQRecv.h"
#include "DboTSQAgency.h"
#include "DboTSTCtrl.h"
#include "DboTSActSToCEvt.h"


/**
	Quest controller
	����Ʈ�� ���� ����, Ŭ���̾�Ʈ ���� ������ �� ���� ��ƾ�� ó���� å���� ����
*/


NTL_TS_IMPLEMENT_RTTI( CDboTSQCtrl, CDboTSTCtrl )


CDboTSQCtrl::CDboTSQCtrl( void )
{
	m_pSavedCurTSP = 0;
	m_pSavedPreTSP = 0;
	m_sSToCEvtData.Init();
	m_tgExcCGroup = NTL_TS_TG_ID_INVALID;

	m_tcQuestInfo = NTL_TS_TC_ID_INVALID;
	m_taQuestInfo = NTL_TS_TA_ID_INVALID;
}

CDboTSQCtrl::~CDboTSQCtrl( void )
{
}

void CDboTSQCtrl::GetSToCEvtDBData( sSTOC_EVT_DB_DATA& sSvrEvtData )
{
#define SAVE_FROM_DB_TO_LOCAL( local, db ) (*(int*)&local = db);

	sSvrEvtData.tcId = m_sSToCEvtData.tcId;
	sSvrEvtData.taId = m_sSToCEvtData.taId;

	if ( NTL_TS_TC_ID_INVALID != m_sSToCEvtData.tcId &&
		 NTL_TS_TA_ID_INVALID != m_sSToCEvtData.taId )
	{
		CNtlTSCont* pCont = GetTrigger()->GetGroup( NTL_TS_MAIN_GROUP_ID )->GetChildCont( m_sSToCEvtData.tcId );
		if ( NULL == pCont )
		{
			CNtlTSLog::Log( "Can not find the SToC Event container. Info[%d, %d]. [%s]", GetTrigger()->GetID(), m_sSToCEvtData.tcId, TS_CODE_TRACE() );

			sSvrEvtData.tcId = NTL_TS_TC_ID_INVALID;
			sSvrEvtData.taId = NTL_TS_TA_ID_INVALID;

			return;
		}

		CNtlTSEntity* pEntity = pCont->FindChildEntity( m_sSToCEvtData.taId );
		if ( NULL == pEntity || DBO_ACT_TYPE_ID_ACT_STOCEVT != pEntity->GetEntityType() )
		{
			CNtlTSLog::Log( "Can not find the SToC Event action. Info[%d, %d]. [%s]", GetTrigger()->GetID(), m_sSToCEvtData.taId, TS_CODE_TRACE() );

			sSvrEvtData.tcId = NTL_TS_TC_ID_INVALID;
			sSvrEvtData.taId = NTL_TS_TA_ID_INVALID;

			return;
		}

		switch ( m_sSToCEvtData.eSToCEvtDataType )
		{
		case eSTOC_EVT_DATA_TYPE_MOB_KILL_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_MOB_KILL; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sMobKillCnt[i].nCurMobCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_MOB_KILL_ITEM_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_MOB_KILL_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sMobKillItemCnt[i].nCurMobLICnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_DELIVERY_ITEM:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_DELIVERY_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sDeliveryItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_OBJECT_ITEM:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_OBJECT_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sObjectItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_PUBLIC_MOB_ITEM_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_PUBLIC_MOB_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sPublicMobItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_CUSTOM_EVT_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_CUSTOM_EVT_CNT; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_DB_TO_LOCAL( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sCustomEvtCnt[i].nCurCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_VISIT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_VISIT_EVT; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					sSvrEvtData.m_aUserData[i] = (m_sSToCEvtData.uSToCEvtData.sVisitEvt[i].bCompleted ? 1 : 0);
				}
			}
			break;

		default:
			{
				CNtlTSLog::Log( "The SToC Evt data type invalid. Info[%d, %d]. [%s]", GetTrigger()->GetID(), m_sSToCEvtData.eSToCEvtDataType, TS_CODE_TRACE() );

				sSvrEvtData.tcId = NTL_TS_TC_ID_INVALID;
				sSvrEvtData.taId = NTL_TS_TA_ID_INVALID;
			}
			return;
		}
	}
	else
	{
		sSvrEvtData.tcId = NTL_TS_TC_ID_INVALID;
		sSvrEvtData.taId = NTL_TS_TA_ID_INVALID;

		memset( sSvrEvtData.m_aUserData, 0, sizeof( sSvrEvtData.m_aUserData ) );
	}
}

void CDboTSQCtrl::SetSToCEvtDBData( const sSTOC_EVT_DB_DATA& sSvrEvtData, bool bIgnoreUserDataField )
{
#define SAVE_FROM_LOCAL_TO_DB( local, db ) (*(int*)&db = local);

	if ( NTL_TS_TC_ID_INVALID == sSvrEvtData.tcId &&
		 NTL_TS_TA_ID_INVALID == sSvrEvtData.taId )
	{
		m_sSToCEvtData.Init();
		return;
	}

	if ( NTL_TS_TC_ID_INVALID != m_sSToCEvtData.tcId ||
		 NTL_TS_TA_ID_INVALID != m_sSToCEvtData.taId )
	{
		CNtlTSLog::Log( "Must be invalid tc and ta id. Info[%d, %d, %d]. [%s]", GetTrigger()->GetID(), sSvrEvtData.tcId, sSvrEvtData.taId, TS_CODE_TRACE() );
	}

	m_sSToCEvtData.tcId = sSvrEvtData.tcId;
	m_sSToCEvtData.taId = sSvrEvtData.taId;

	CNtlTSCont* pCont = GetTrigger()->GetGroup( NTL_TS_MAIN_GROUP_ID )->GetChildCont( sSvrEvtData.tcId );
	if ( NULL == pCont )
	{
		CNtlTSLog::Log( "Can not find the SToC Event container. Info[%d, %d]. [%s]", GetTrigger()->GetID(), sSvrEvtData.tcId, TS_CODE_TRACE() );

		m_sSToCEvtData.tcId = NTL_TS_TC_ID_INVALID;
		m_sSToCEvtData.taId = NTL_TS_TA_ID_INVALID;

		return;
	}

	CNtlTSEntity* pEntity = pCont->FindChildEntity( sSvrEvtData.taId );
	if ( NULL == pEntity || DBO_ACT_TYPE_ID_ACT_STOCEVT != pEntity->GetEntityType() )
	{
		CNtlTSLog::Log( "Can not find the SToC Event action. Info[%d, %d]. [%s]", GetTrigger()->GetID(), sSvrEvtData.taId, TS_CODE_TRACE() );

		m_sSToCEvtData.tcId = NTL_TS_TC_ID_INVALID;
		m_sSToCEvtData.taId = NTL_TS_TA_ID_INVALID;

		return;
	}

	CDboTSActSToCEvt* pSToCEvt = (CDboTSActSToCEvt*) pEntity;

	m_sSToCEvtData.eSToCEvtDataType = pSToCEvt->GetEvtDataType();
	memcpy( &m_sSToCEvtData.uSToCEvtData, &pSToCEvt->GetEvtData(), sizeof( uSTOC_EVT_DATA ) );

	m_sSToCEvtData.eSToCEvtCondDataType = pSToCEvt->GetEvtCondDataType();
	memcpy( &m_sSToCEvtData.uSToCEvtCondData, &pSToCEvt->GetEvtCondData(), sizeof( uSTOC_EVT_COND_DATA ) );

	m_sSToCEvtData.uiSToCEvtSubCondDataType = pSToCEvt->GetEvtSubCondDataType();
	memcpy( &m_sSToCEvtData.sSToCEvtSubCondData, &pSToCEvt->GetEvtSubCondData(), sizeof( sSTOC_EVT_SUB_COND_DATA ) );

	if ( !bIgnoreUserDataField )
	{
		switch ( m_sSToCEvtData.eSToCEvtDataType )
		{
		case eSTOC_EVT_DATA_TYPE_MOB_KILL_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_MOB_KILL; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sMobKillCnt[i].nCurMobCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_MOB_KILL_ITEM_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_MOB_KILL_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sMobKillItemCnt[i].nCurMobLICnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_DELIVERY_ITEM:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_DELIVERY_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sDeliveryItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_OBJECT_ITEM:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_OBJECT_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sObjectItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_PUBLIC_MOB_ITEM_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_PUBLIC_MOB_ITEM; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sPublicMobItemCnt[i].nCurItemCnt );
				}
			}
			break;
		case eSTOC_EVT_DATA_TYPE_CUSTOM_EVT_CNT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_CUSTOM_EVT_CNT; ++i )
				{
					// �����ϰ��� �ϴ� �����Ͱ� Int ���� ��� ( 4����Ʈ �� �Ѿ )
					SAVE_FROM_LOCAL_TO_DB( sSvrEvtData.m_aUserData[i], m_sSToCEvtData.uSToCEvtData.sCustomEvtCnt[i].nCurCnt );
				}
			}
			break;

		case eSTOC_EVT_DATA_TYPE_VISIT:
			{
				for ( int i = 0; i < uSTOC_EVT_DATA::MAX_VISIT_EVT; ++i )
				{
					m_sSToCEvtData.uSToCEvtData.sVisitEvt[i].bCompleted = (sSvrEvtData.m_aUserData[i] ? true : false);
				}
			}
			break;

		default:
			{
				CNtlTSLog::Log( "The SToC Evt data type invalid. Info[%d, %d]. [%s]", GetTrigger()->GetID(), m_sSToCEvtData.eSToCEvtDataType, TS_CODE_TRACE() );

				m_sSToCEvtData.tcId = NTL_TS_TC_ID_INVALID;
				m_sSToCEvtData.taId = NTL_TS_TA_ID_INVALID;
			}
			return;
		}
	}
}

void CDboTSQCtrl::SetExcCGroup( NTL_TS_TG_ID tgId )
{
	m_tgExcCGroup = tgId;
}

void CDboTSQCtrl::SetQuestInfo( NTL_TS_TC_ID tcID, NTL_TS_TA_ID taID )
{
	m_tcQuestInfo = tcID;
	m_taQuestInfo = taID;
}

void CDboTSQCtrl::LoadQuestProgressInfo_V0( const sPROGRESS_QUEST_INFO::uDATA& uData )
{
	int i;

	// ����Ʈ ���� �ε�
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_ERROR )
	{
		SetError( true, false );
	}
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_FAILED )
	{
		SetFailed( true, false );
	}
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_ESCORT )
	{
		SetEscort( true, false );
		SetFailed( true, false );	// �ε� ������ Escort �÷��װ� �� �ִ� ���� ������ ����Ʈ ��.
	}
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_SVR_EVT_WORLD_RANGE )
	{
		SetSvrEvtWorldRange( true, false );
	}
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_WPS_MODE )
	{
		SetWPSMode( true, false );
	}
	if ( uData.sQInfoV0.wQState & eTS_SVR_STATE_USER_OUT_FAILED_WPS )
	{
		SetUserOutFailedWPS( true, false );
		SetFailed( true, false );	// �ε� ������ World play script���� User out failed �÷��׸� �� ���� ������ ����Ʈ ��.
	}

	// ����Ʈ ���� ���� �ε�
	m_pSavedCurTSP = GetTrigger()->GetGroup( NTL_TS_MAIN_GROUP_ID )->GetChildCont( uData.sQInfoV0.sMainTSP.tcCurId );
	m_pSavedPreTSP = GetTrigger()->GetGroup( NTL_TS_MAIN_GROUP_ID )->GetChildCont( uData.sQInfoV0.sMainTSP.tcPreId );
	m_pCurTSP = m_pSavedCurTSP;

	// ���� Ÿ�̸� �ε�
	for ( i = 0; i < sEXCEPT_TIMER_SLOT::eTIMER_SLOT_MAX; ++i )
	{
		if ( NTL_TS_TC_ID_INVALID != uData.sQInfoV0.sETSlot.asExceptTimer[i].tcId &&
			 NTL_TS_TA_ID_INVALID != uData.sQInfoV0.sETSlot.asExceptTimer[i].taId )
		{
			unsigned int uiCurTime = GetCurTime();

			AttachExceptionTimer( uData.sQInfoV0.sETSlot.asExceptTimer[i].tcId,
								  uData.sQInfoV0.sETSlot.asExceptTimer[i].taId,
								  uiCurTime,
								  uData.sQInfoV0.sETSlot.asExceptTimer[i].uiRemainTime );
		}
	}

	// ���� ���� �޸� �ε�
	memcpy( &m_sSSM, &uData.sQInfoV0.sSSM, sizeof( sSTORAGE_SLOT_MEMORY ) );

	// ���� �̺�Ʈ �ε�
	SetSToCEvtDBData( uData.sQInfoV0.sSToCEvtData, false );

	// ������ Ŭ���̾�Ʈ �׷�
	m_tgExcCGroup = uData.sQInfoV0.tgExcCGroup;

	// ����Ʈ ����
	m_tcQuestInfo = uData.sQInfoV0.tcQuestInfo;
	m_taQuestInfo = uData.sQInfoV0.taQuestInfo;
}

void CDboTSQCtrl::SaveQuestProgressInfo_V0( sPROGRESS_QUEST_INFO::uDATA& uData )
{
	int i;

	// ����Ʈ ���� ����
	uData.sQInfoV0.wQState = 0;
	if ( IsError() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_ERROR;
	}
	if ( IsFailed() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_FAILED;
	}
	if ( IsEscort() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_ESCORT;
	}
	if ( IsSvrEvtWorldRange() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_SVR_EVT_WORLD_RANGE;
	}
	if ( IsWPSMode() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_WPS_MODE;
	}
	if ( IsUserOutFailedWPS() )
	{
		uData.sQInfoV0.wQState |= eTS_SVR_STATE_USER_OUT_FAILED_WPS;
	}

	// ����Ʈ ���� ���� ����
	uData.sQInfoV0.sMainTSP.tcCurId = m_pSavedCurTSP ? m_pSavedCurTSP->GetID() : NTL_TS_TC_ID_INVALID;
	uData.sQInfoV0.sMainTSP.tcPreId = m_pSavedPreTSP ? m_pSavedPreTSP->GetID() : NTL_TS_TC_ID_INVALID;

	// ���� Ÿ�̸� ����
	memset( &uData.sQInfoV0.sETSlot, 0xff, sizeof(sEXCEPT_TIMER_SLOT) );
	mapdef_EXCEPT_TIMER_SLOT::iterator itETimer = m_defExceptTimerSlot.begin();
	for ( i = 0; itETimer != m_defExceptTimerSlot.end(); ++itETimer, ++i )
	{
		sEXCEPTION_TIMER& sTimer = itETimer->second;

		uData.sQInfoV0.sETSlot.asExceptTimer[i].tcId = sTimer.tcId;
		uData.sQInfoV0.sETSlot.asExceptTimer[i].taId = sTimer.taId;
		uData.sQInfoV0.sETSlot.asExceptTimer[i].uiRemainTime = sTimer.uiRemainTime;
	}

	// ���� ���� �޸� ����
	memcpy( &uData.sQInfoV0.sSSM, &m_sSSM, sizeof( sSTORAGE_SLOT_MEMORY ) );

	// ���� �̺�Ʈ ����
	GetSToCEvtDBData( uData.sQInfoV0.sSToCEvtData );

	// ������ Ŭ���̾�Ʈ �׷�
	uData.sQInfoV0.tgExcCGroup = m_tgExcCGroup;

	// ����Ʈ ����
	uData.sQInfoV0.tcQuestInfo = m_tcQuestInfo;
	uData.sQInfoV0.taQuestInfo = m_taQuestInfo;
}

void CDboTSQCtrl::MoveTSP( CNtlTSCont* pCurCont, CNtlTSCont* pNextCont, bool bSave )
{
    pCurCont;

	if ( bSave )
	{
		m_pSavedPreTSP = m_pSavedCurTSP;
		m_pSavedCurTSP = pNextCont;
	}

	m_pCurTSP = pNextCont;
}
