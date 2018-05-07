// DlgSSSDT.cpp : 实现文件
//

#include "stdafx.h"
#include "ZhuHuiBeiShaDiaoARK.h"
#include "DlgSSSDT.h"
#include "afxdialogex.h"

typedef struct _SSSDT_INFO
{
	ULONG64 Address;
	CHAR	ImgPath[MAX_PATH];
}SSSDT_INFO, *PSSSDT_INFO;

PCHAR SssdtFunWin7x64[1000]
{
	"NtUserGetThreadState"
	,"NtUserPeekMessage"
	,"NtUserCallOneParam"
	,"NtUserGetKeyState"
	,"NtUserInvalidateRect"
	,"NtUserCallNoParam"
	,"NtUserGetMessage"
	,"NtUserMessageCall"
	,"NtGdiBitBlt"
	,"NtGdiGetCharSet"
	,"NtUserGetDC"
	,"NtGdiSelectBitmap"
	,"NtUserWaitMessage"
	,"NtUserTranslateMessage"
	,"NtUserGetProp"
	,"NtUserPostMessage"
	,"NtUserQueryWindow"
	,"NtUserTranslateAccelerator"
	,"NtGdiFlush"
	,"NtUserRedrawWindow"
	,"NtUserWindowFromPoint"
	,"NtUserCallMsgFilter"
	,"NtUserValidateTimerCallback"
	,"NtUserBeginPaint"
	,"NtUserSetTimer"
	,"NtUserEndPaint"
	,"NtUserSetCursor"
	,"NtUserKillTimer"
	,"NtUserBuildHwndList"
	,"NtUserSelectPalette"
	,"NtUserCallNextHookEx"
	,"NtUserHideCaret"
	,"NtGdiIntersectClipRect"
	,"NtUserCallHwndLock"
	,"NtUserGetProcessWindowStation"
	,"NtGdiDeleteObjectApp"
	,"NtUserSetWindowPos"
	,"NtUserShowCaret"
	,"NtUserEndDeferWindowPosEx"
	,"NtUserCallHwndParamLock"
	,"NtUserVkKeyScanEx"
	,"NtGdiSetDIBitsToDeviceInternal"
	,"NtUserCallTwoParam"
	,"NtGdiGetRandomRgn"
	,"NtUserCopyAcceleratorTable"
	,"NtUserNotifyWinEvent"
	,"NtGdiExtSelectClipRgn"
	,"NtUserIsClipboardFormatAvailable"
	,"NtUserSetScrollInfo"
	,"NtGdiStretchBlt"
	,"NtUserCreateCaret"
	,"NtGdiRectVisible"
	,"NtGdiCombineRgn"
	,"NtGdiGetDCObject"
	,"NtUserDispatchMessage"
	,"NtUserRegisterWindowMessage"
	,"NtGdiExtTextOutW"
	,"NtGdiSelectFont"
	,"NtGdiRestoreDC"
	,"NtGdiSaveDC"
	,"NtUserGetForegroundWindow"
	,"NtUserShowScrollBar"
	,"NtUserFindExistingCursorIcon"
	,"NtGdiGetDCDword"
	,"NtGdiGetRegionData"
	,"NtGdiLineTo"
	,"NtUserSystemParametersInfo"
	,"NtGdiGetAppClipBox"
	,"NtUserGetAsyncKeyState"
	,"NtUserGetCPD"
	,"NtUserRemoveProp"
	,"NtGdiDoPalette"
	,"NtGdiPolyPolyDraw"
	,"NtUserSetCapture"
	,"NtUserEnumDisplayMonitors"
	,"NtGdiCreateCompatibleBitmap"
	,"NtUserSetProp"
	,"NtGdiGetTextCharsetInfo"
	,"NtUserSBGetParms"
	,"NtUserGetIconInfo"
	,"NtUserExcludeUpdateRgn"
	,"NtUserSetFocus"
	,"NtGdiExtGetObjectW"
	,"NtUserDeferWindowPos"
	,"NtUserGetUpdateRect"
	,"NtGdiCreateCompatibleDC"
	,"NtUserGetClipboardSequenceNumber"
	,"NtGdiCreatePen"
	,"NtUserShowWindow"
	,"NtUserGetKeyboardLayoutList"
	,"NtGdiPatBlt"
	,"NtUserMapVirtualKeyEx"
	,"NtUserSetWindowLong"
	,"NtGdiHfontCreate"
	,"NtUserMoveWindow"
	,"NtUserPostThreadMessage"
	,"NtUserDrawIconEx"
	,"NtUserGetSystemMenu"
	,"NtGdiDrawStream"
	,"NtUserInternalGetWindowText"
	,"NtUserGetWindowDC"
	,"NtGdiD3dDrawPrimitives2"
	,"NtGdiInvertRgn"
	,"NtGdiGetRgnBox"
	,"NtGdiGetAndSetDCDword"
	,"NtGdiMaskBlt"
	,"NtGdiGetWidthTable"
	,"NtUserScrollDC"
	,"NtUserGetObjectInformation"
	,"NtGdiCreateBitmap"
	,"NtUserFindWindowEx"
	,"NtGdiPolyPatBlt"
	,"NtUserUnhookWindowsHookEx"
	,"NtGdiGetNearestColor"
	,"NtGdiTransformPoints"
	,"NtGdiGetDCPoint"
	,"NtGdiCreateDIBBrush"
	,"NtGdiGetTextMetricsW"
	,"NtUserCreateWindowEx"
	,"NtUserSetParent"
	,"NtUserGetKeyboardState"
	,"NtUserToUnicodeEx"
	,"NtUserGetControlBrush"
	,"NtUserGetClassName"
	,"NtGdiAlphaBlend"
	,"NtGdiDdBlt"
	,"NtGdiOffsetRgn"
	,"NtUserDefSetText"
	,"NtGdiGetTextFaceW"
	,"NtGdiStretchDIBitsInternal"
	,"NtUserSendInput"
	,"NtUserGetThreadDesktop"
	,"NtGdiCreateRectRgn"
	,"NtGdiGetDIBitsInternal"
	,"NtUserGetUpdateRgn"
	,"NtGdiDeleteClientObj"
	,"NtUserGetIconSize"
	,"NtUserFillWindow"
	,"NtGdiExtCreateRegion"
	,"NtGdiComputeXformCoefficients"
	,"NtUserSetWindowsHookEx"
	,"NtUserNotifyProcessCreate"
	,"NtGdiUnrealizeObject"
	,"NtUserGetTitleBarInfo"
	,"NtGdiRectangle"
	,"NtUserSetThreadDesktop"
	,"NtUserGetDCEx"
	,"NtUserGetScrollBarInfo"
	,"NtGdiGetTextExtent"
	,"NtUserSetWindowFNID"
	,"NtGdiSetLayout"
	,"NtUserCalcMenuBar"
	,"NtUserThunkedMenuItemInfo"
	,"NtGdiExcludeClipRect"
	,"NtGdiCreateDIBSection"
	,"NtGdiGetDCforBitmap"
	,"NtUserDestroyCursor"
	,"NtUserDestroyWindow"
	,"NtUserCallHwndParam"
	,"NtGdiCreateDIBitmapInternal"
	,"NtUserOpenWindowStation"
	,"NtGdiDdDeleteSurfaceObject"
	,"NtGdiDdCanCreateSurface"
	,"NtGdiDdCreateSurface"
	,"NtUserSetCursorIconData"
	,"NtGdiDdDestroySurface"
	,"NtUserCloseDesktop"
	,"NtUserOpenDesktop"
	,"NtUserSetProcessWindowStation"
	,"NtUserGetAtomName"
	,"NtGdiDdResetVisrgn"
	,"NtGdiExtCreatePen"
	,"NtGdiCreatePaletteInternal"
	,"NtGdiSetBrushOrg"
	,"NtUserBuildNameList"
	,"NtGdiSetPixel"
	,"NtUserRegisterClassExWOW"
	,"NtGdiCreatePatternBrushInternal"
	,"NtUserGetAncestor"
	,"NtGdiGetOutlineTextMetricsInternalW"
	,"NtGdiSetBitmapBits"
	,"NtUserCloseWindowStation"
	,"NtUserGetDoubleClickTime"
	,"NtUserEnableScrollBar"
	,"NtGdiCreateSolidBrush"
	,"NtUserGetClassInfoEx"
	,"NtGdiCreateClientObj"
	,"NtUserUnregisterClass"
	,"NtUserDeleteMenu"
	,"NtGdiRectInRegion"
	,"NtUserScrollWindowEx"
	,"NtGdiGetPixel"
	,"NtUserSetClassLong"
	,"NtUserGetMenuBarInfo"
	,"NtGdiDdCreateSurfaceEx"
	,"NtGdiDdCreateSurfaceObject"
	,"NtGdiGetNearestPaletteIndex"
	,"NtGdiDdLockD3D"
	,"NtGdiDdUnlockD3D"
	,"NtGdiGetCharWidthW"
	,"NtUserInvalidateRgn"
	,"NtUserGetClipboardOwner"
	,"NtUserSetWindowRgn"
	,"NtUserBitBltSysBmp"
	,"NtGdiGetCharWidthInfo"
	,"NtUserValidateRect"
	,"NtUserCloseClipboard"
	,"NtUserOpenClipboard"
	,"NtGdiGetStockObject"
	,"NtUserSetClipboardData"
	,"NtUserEnableMenuItem"
	,"NtUserAlterWindowStyle"
	,"NtGdiFillRgn"
	,"NtUserGetWindowPlacement"
	,"NtGdiModifyWorldTransform"
	,"NtGdiGetFontData"
	,"NtUserGetOpenClipboardWindow"
	,"NtUserSetThreadState"
	,"NtGdiOpenDCW"
	,"NtUserTrackMouseEvent"
	,"NtGdiGetTransform"
	,"NtUserDestroyMenu"
	,"NtGdiGetBitmapBits"
	,"NtUserConsoleControl"
	,"NtUserSetActiveWindow"
	,"NtUserSetInformationThread"
	,"NtUserSetWindowPlacement"
	,"NtUserGetControlColor"
	,"NtGdiSetMetaRgn"
	,"NtGdiSetMiterLimit"
	,"NtGdiSetVirtualResolution"
	,"NtGdiGetRasterizerCaps"
	,"NtUserSetWindowWord"
	,"NtUserGetClipboardFormatName"
	,"NtUserRealInternalGetMessage"
	,"NtUserCreateLocalMemHandle"
	,"NtUserAttachThreadInput"
	,"NtGdiCreateHalftonePalette"
	,"NtUserPaintMenuBar"
	,"NtUserSetKeyboardState"
	,"NtGdiCombineTransform"
	,"NtUserCreateAcceleratorTable"
	,"NtUserGetCursorFrameInfo"
	,"NtUserGetAltTabInfo"
	,"NtUserGetCaretBlinkTime"
	,"NtGdiQueryFontAssocInfo"
	,"NtUserProcessConnect"
	,"NtUserEnumDisplayDevices"
	,"NtUserEmptyClipboard"
	,"NtUserGetClipboardData"
	,"NtUserRemoveMenu"
	,"NtGdiSetBoundsRect"
	,"NtGdiGetBitmapDimension"
	,"NtUserConvertMemHandle"
	,"NtUserDestroyAcceleratorTable"
	,"NtUserGetGUIThreadInfo"
	,"NtGdiCloseFigure"
	,"NtUserSetWindowsHookAW"
	,"NtUserSetMenuDefaultItem"
	,"NtUserCheckMenuItem"
	,"NtUserSetWinEventHook"
	,"NtUserUnhookWinEvent"
	,"NtUserLockWindowUpdate"
	,"NtUserSetSystemMenu"
	,"NtUserThunkedMenuInfo"
	,"NtGdiBeginPath"
	,"NtGdiEndPath"
	,"NtGdiFillPath"
	,"NtUserCallHwnd"
	,"NtUserDdeInitialize"
	,"NtUserModifyUserStartupInfoFlags"
	,"NtUserCountClipboardFormats"
	,"NtGdiAddFontMemResourceEx"
	,"NtGdiEqualRgn"
	,"NtGdiGetSystemPaletteUse"
	,"NtGdiRemoveFontMemResourceEx"
	,"NtUserEnumDisplaySettings"
	,"NtUserPaintDesktop"
	,"NtGdiExtEscape"
	,"NtGdiSetBitmapDimension"
	,"NtGdiSetFontEnumeration"
	,"NtUserChangeClipboardChain"
	,"NtUserSetClipboardViewer"
	,"NtUserShowWindowAsync"
	,"NtGdiCreateColorSpace"
	,"NtGdiDeleteColorSpace"
	,"NtUserActivateKeyboardLayout"
	,"NtGdiAbortDoc"
	,"NtGdiAbortPath"
	,"NtGdiAddEmbFontToDC"
	,"NtGdiAddFontResourceW"
	,"NtGdiAddRemoteFontToDC"
	,"NtGdiAddRemoteMMInstanceToDC"
	,"NtGdiAngleArc"
	,"NtGdiAnyLinkedFonts"
	,"NtGdiArcInternal"
	,"NtGdiBRUSHOBJ_DeleteRbrush"
	,"NtGdiBRUSHOBJ_hGetColorTransform"
	,"NtGdiBRUSHOBJ_pvAllocRbrush"
	,"NtGdiBRUSHOBJ_pvGetRbrush"
	,"NtGdiBRUSHOBJ_ulGetBrushColor"
	,"NtGdiBeginGdiRendering"
	,"NtGdiCLIPOBJ_bEnum"
	,"NtGdiCLIPOBJ_cEnumStart"
	,"NtGdiCLIPOBJ_ppoGetPath"
	,"NtGdiCancelDC"
	,"NtGdiChangeGhostFont"
	,"NtGdiCheckBitmapBits"
	,"NtGdiClearBitmapAttributes"
	,"NtGdiClearBrushAttributes"
	,"NtGdiColorCorrectPalette"
	,"NtGdiConfigureOPMProtectedOutput"
	,"NtGdiConvertMetafileRect"
	,"NtGdiCreateBitmapFromDxSurface"
	,"NtGdiCreateColorTransform"
	,"NtGdiCreateEllipticRgn"
	,"NtGdiCreateHatchBrushInternal"
	,"NtGdiCreateMetafileDC"
	,"NtGdiCreateOPMProtectedOutputs"
	,"NtGdiCreateRoundRectRgn"
	,"NtGdiCreateServerMetaFile"
	,"NtGdiD3dContextCreate"
	,"NtGdiD3dContextDestroy"
	,"NtGdiD3dContextDestroyAll"
	,"NtGdiD3dValidateTextureStageState"
	,"NtGdiDDCCIGetCapabilitiesString"
	,"NtGdiDDCCIGetCapabilitiesStringLength"
	,"NtGdiDDCCIGetTimingReport"
	,"NtGdiDDCCIGetVCPFeature"
	,"NtGdiDDCCISaveCurrentSettings"
	,"NtGdiDDCCISetVCPFeature"
	,"NtGdiDdAddAttachedSurface"
	,"NtGdiDdAlphaBlt"
	,"NtGdiDdAttachSurface"
	,"NtGdiDdBeginMoCompFrame"
	,"NtGdiDdCanCreateD3DBuffer"
	,"NtGdiDdColorControl"
	,"NtGdiDdCreateD3DBuffer"
	,"NtGdiDdCreateDirectDrawObject"
	,"NtGdiDdCreateFullscreenSprite"
	,"NtGdiDdCreateMoComp"
	,"NtGdiDdDDIAcquireKeyedMutex"
	,"NtGdiDdDDICheckExclusiveOwnership"
	,"NtGdiDdDDICheckMonitorPowerState"
	,"NtGdiDdDDICheckOcclusion"
	,"NtGdiDdDDICheckSharedResourceAccess"
	,"NtGdiDdDDICheckVidPnExclusiveOwnership"
	,"NtGdiDdDDICloseAdapter"
	,"NtGdiDdDDIConfigureSharedResource"
	,"NtGdiDdDDICreateAllocation"
	,"NtGdiDdDDICreateContext"
	,"NtGdiDdDDICreateDCFromMemory"
	,"NtGdiDdDDICreateDevice"
	,"NtGdiDdDDICreateKeyedMutex"
	,"NtGdiDdDDICreateOverlay"
	,"NtGdiDdDDICreateSynchronizationObject"
	,"NtGdiDdDDIDestroyAllocation"
	,"NtGdiDdDDIDestroyContext"
	,"NtGdiDdDDIDestroyDCFromMemory"
	,"NtGdiDdDDIDestroyDevice"
	,"NtGdiDdDDIDestroyKeyedMutex"
	,"NtGdiDdDDIDestroyOverlay"
	,"NtGdiDdDDIDestroySynchronizationObject"
	,"NtGdiDdDDIEscape"
	,"NtGdiDdDDIFlipOverlay"
	,"NtGdiDdDDIGetContextSchedulingPriority"
	,"NtGdiDdDDIGetDeviceState"
	,"NtGdiDdDDIGetDisplayModeList"
	,"NtGdiDdDDIGetMultisampleMethodList"
	,"NtGdiDdDDIGetOverlayState"
	,"NtGdiDdDDIGetPresentHistory"
	,"NtGdiDdDDIGetPresentQueueEvent"
	,"NtGdiDdDDIGetProcessSchedulingPriorityClass"
	,"NtGdiDdDDIGetRuntimeData"
	,"NtGdiDdDDIGetScanLine"
	,"NtGdiDdDDIGetSharedPrimaryHandle"
	,"NtGdiDdDDIInvalidateActiveVidPn"
	,"NtGdiDdDDILock"
	,"NtGdiDdDDIOpenAdapterFromDeviceName"
	,"NtGdiDdDDIOpenAdapterFromHdc"
	,"NtGdiDdDDIOpenKeyedMutex"
	,"NtGdiDdDDIOpenResource"
	,"NtGdiDdDDIOpenSynchronizationObject"
	,"NtGdiDdDDIPollDisplayChildren"
	,"NtGdiDdDDIPresent"
	,"NtGdiDdDDIQueryAdapterInfo"
	,"NtGdiDdDDIQueryAllocationResidency"
	,"NtGdiDdDDIQueryResourceInfo"
	,"NtGdiDdDDIQueryStatistics"
	,"NtGdiDdDDIReleaseKeyedMutex"
	,"NtGdiDdDDIReleaseProcessVidPnSourceOwners"
	,"NtGdiDdDDIRender"
	,"NtGdiDdDDISetAllocationPriority"
	,"NtGdiDdDDISetContextSchedulingPriority"
	,"NtGdiDdDDISetDisplayMode"
	,"NtGdiDdDDISetDisplayPrivateDriverFormat"
	,"NtGdiDdDDISetGammaRamp"
	,"NtGdiDdDDISetProcessSchedulingPriorityClass"
	,"NtGdiDdDDISetQueuedLimit"
	,"NtGdiDdDDISetVidPnSourceOwner"
	,"NtGdiDdDDISharedPrimaryLockNotification"
	,"NtGdiDdDDISharedPrimaryUnLockNotification"
	,"NtGdiDdDDISignalSynchronizationObject"
	,"NtGdiDdDDIUnlock"
	,"NtGdiDdDDIUpdateOverlay"
	,"NtGdiDdDDIWaitForIdle"
	,"NtGdiDdDDIWaitForSynchronizationObject"
	,"NtGdiDdDDIWaitForVerticalBlankEvent"
	,"NtGdiDdDeleteDirectDrawObject"
	,"NtGdiDdDestroyD3DBuffer"
	,"NtGdiDdDestroyFullscreenSprite"
	,"NtGdiDdDestroyMoComp"
	,"NtGdiDdEndMoCompFrame"
	,"NtGdiDdFlip"
	,"NtGdiDdFlipToGDISurface"
	,"NtGdiDdGetAvailDriverMemory"
	,"NtGdiDdGetBltStatus"
	,"NtGdiDdGetDC"
	,"NtGdiDdGetDriverInfo"
	,"NtGdiDdGetDriverState"
	,"NtGdiDdGetDxHandle"
	,"NtGdiDdGetFlipStatus"
	,"NtGdiDdGetInternalMoCompInfo"
	,"NtGdiDdGetMoCompBuffInfo"
	,"NtGdiDdGetMoCompFormats"
	,"NtGdiDdGetMoCompGuids"
	,"NtGdiDdGetScanLine"
	,"NtGdiDdLock"
	,"NtGdiDdNotifyFullscreenSpriteUpdate"
	,"NtGdiDdQueryDirectDrawObject"
	,"NtGdiDdQueryMoCompStatus"
	,"NtGdiDdQueryVisRgnUniqueness"
	,"NtGdiDdReenableDirectDrawObject"
	,"NtGdiDdReleaseDC"
	,"NtGdiDdRenderMoComp"
	,"NtGdiDdSetColorKey"
	,"NtGdiDdSetExclusiveMode"
	,"NtGdiDdSetGammaRamp"
	,"NtGdiDdSetOverlayPosition"
	,"NtGdiDdUnattachSurface"
	,"NtGdiDdUnlock"
	,"NtGdiDdUpdateOverlay"
	,"NtGdiDdWaitForVerticalBlank"
	,"NtGdiDeleteColorTransform"
	,"NtGdiDescribePixelFormat"
	,"NtGdiDestroyOPMProtectedOutput"
	,"NtGdiDestroyPhysicalMonitor"
	,"NtGdiDoBanding"
	,"NtGdiDrawEscape"
	,"NtGdiDvpAcquireNotification"
	,"NtGdiDvpCanCreateVideoPort"
	,"NtGdiDvpColorControl"
	,"NtGdiDvpCreateVideoPort"
	,"NtGdiDvpDestroyVideoPort"
	,"NtGdiDvpFlipVideoPort"
	,"NtGdiDvpGetVideoPortBandwidth"
	,"NtGdiDvpGetVideoPortConnectInfo"
	,"NtGdiDvpGetVideoPortField"
	,"NtGdiDvpGetVideoPortFlipStatus"
	,"NtGdiDvpGetVideoPortInputFormats"
	,"NtGdiDvpGetVideoPortLine"
	,"NtGdiDvpGetVideoPortOutputFormats"
	,"NtGdiDvpGetVideoSignalStatus"
	,"NtGdiDvpReleaseNotification"
	,"NtGdiDvpUpdateVideoPort"
	,"NtGdiDvpWaitForVideoPortSync"
	,"NtGdiDxgGenericThunk"
	,"NtGdiEllipse"
	,"NtGdiEnableEudc"
	,"NtGdiEndDoc"
	,"NtGdiEndGdiRendering"
	,"NtGdiEndPage"
	,"NtGdiEngAlphaBlend"
	,"NtGdiEngAssociateSurface"
	,"NtGdiEngBitBlt"
	,"NtGdiEngCheckAbort"
	,"NtGdiEngComputeGlyphSet"
	,"NtGdiEngCopyBits"
	,"NtGdiEngCreateBitmap"
	,"NtGdiEngCreateClip"
	,"NtGdiEngCreateDeviceBitmap"
	,"NtGdiEngCreateDeviceSurface"
	,"NtGdiEngCreatePalette"
	,"NtGdiEngDeleteClip"
	,"NtGdiEngDeletePalette"
	,"NtGdiEngDeletePath"
	,"NtGdiEngDeleteSurface"
	,"NtGdiEngEraseSurface"
	,"NtGdiEngFillPath"
	,"NtGdiEngGradientFill"
	,"NtGdiEngLineTo"
	,"NtGdiEngLockSurface"
	,"NtGdiEngMarkBandingSurface"
	,"NtGdiEngPaint"
	,"NtGdiEngPlgBlt"
	,"NtGdiEngStretchBlt"
	,"NtGdiEngStretchBltROP"
	,"NtGdiEngStrokeAndFillPath"
	,"NtGdiEngStrokePath"
	,"NtGdiEngTextOut"
	,"NtGdiEngTransparentBlt"
	,"NtGdiEngUnlockSurface"
	,"NtGdiEnumFonts"
	,"NtGdiEnumObjects"
	,"NtGdiEudcLoadUnloadLink"
	,"NtGdiExtFloodFill"
	,"NtGdiFONTOBJ_cGetAllGlyphHandles"
	,"NtGdiFONTOBJ_cGetGlyphs"
	,"NtGdiFONTOBJ_pQueryGlyphAttrs"
	,"NtGdiFONTOBJ_pfdg"
	,"NtGdiFONTOBJ_pifi"
	,"NtGdiFONTOBJ_pvTrueTypeFontFile"
	,"NtGdiFONTOBJ_pxoGetXform"
	,"NtGdiFONTOBJ_vGetInfo"
	,"NtGdiFlattenPath"
	,"NtGdiFontIsLinked"
	,"NtGdiForceUFIMapping"
	,"NtGdiFrameRgn"
	,"NtGdiFullscreenControl"
	,"NtGdiGetBoundsRect"
	,"NtGdiGetCOPPCompatibleOPMInformation"
	,"NtGdiGetCertificate"
	,"NtGdiGetCertificateSize"
	,"NtGdiGetCharABCWidthsW"
	,"NtGdiGetCharacterPlacementW"
	,"NtGdiGetColorAdjustment"
	,"NtGdiGetColorSpaceforBitmap"
	,"NtGdiGetDeviceCaps"
	,"NtGdiGetDeviceCapsAll"
	,"NtGdiGetDeviceGammaRamp"
	,"NtGdiGetDeviceWidth"
	,"NtGdiGetDhpdev"
	,"NtGdiGetETM"
	,"NtGdiGetEmbUFI"
	,"NtGdiGetEmbedFonts"
	,"NtGdiGetEudcTimeStampEx"
	,"NtGdiGetFontFileData"
	,"NtGdiGetFontFileInfo"
	,"NtGdiGetFontResourceInfoInternalW"
	,"NtGdiGetFontUnicodeRanges"
	,"NtGdiGetGlyphIndicesW"
	,"NtGdiGetGlyphIndicesWInternal"
	,"NtGdiGetGlyphOutline"
	,"NtGdiGetKerningPairs"
	,"NtGdiGetLinkedUFIs"
	,"NtGdiGetMiterLimit"
	,"NtGdiGetMonitorID"
	,"NtGdiGetNumberOfPhysicalMonitors"
	,"NtGdiGetOPMInformation"
	,"NtGdiGetOPMRandomNumber"
	,"NtGdiGetObjectBitmapHandle"
	,"NtGdiGetPath"
	,"NtGdiGetPerBandInfo"
	,"NtGdiGetPhysicalMonitorDescription"
	,"NtGdiGetPhysicalMonitors"
	,"NtGdiGetRealizationInfo"
	,"NtGdiGetServerMetaFileBits"
	,"DxgStubAlphaBlt"
	,"NtGdiGetStats"
	,"NtGdiGetStringBitmapW"
	,"NtGdiGetSuggestedOPMProtectedOutputArraySize"
	,"NtGdiGetTextExtentExW"
	,"NtGdiGetUFI"
	,"NtGdiGetUFIPathname"
	,"NtGdiGradientFill"
	,"NtGdiHLSurfGetInformation"
	,"NtGdiHLSurfSetInformation"
	,"NtGdiHT_Get8BPPFormatPalette"
	,"NtGdiHT_Get8BPPMaskPalette"
	,"NtGdiIcmBrushInfo"
	,"EngRestoreFloatingPointState"
	,"NtGdiInitSpool"
	,"NtGdiMakeFontDir"
	,"NtGdiMakeInfoDC"
	,"NtGdiMakeObjectUnXferable"
	,"NtGdiMakeObjectXferable"
	,"NtGdiMirrorWindowOrg"
	,"NtGdiMonoBitmap"
	,"NtGdiMoveTo"
	,"NtGdiOffsetClipRgn"
	,"NtGdiPATHOBJ_bEnum"
	,"NtGdiPATHOBJ_bEnumClipLines"
	,"NtGdiPATHOBJ_vEnumStart"
	,"NtGdiPATHOBJ_vEnumStartClipLines"
	,"NtGdiPATHOBJ_vGetBounds"
	,"NtGdiPathToRegion"
	,"NtGdiPlgBlt"
	,"NtGdiPolyDraw"
	,"NtGdiPolyTextOutW"
	,"NtGdiPtInRegion"
	,"NtGdiPtVisible"
	,"NtGdiQueryFonts"
	,"NtGdiRemoveFontResourceW"
	,"NtGdiRemoveMergeFont"
	,"NtGdiResetDC"
	,"NtGdiResizePalette"
	,"NtGdiRoundRect"
	,"NtGdiSTROBJ_bEnum"
	,"NtGdiSTROBJ_bEnumPositionsOnly"
	,"NtGdiSTROBJ_bGetAdvanceWidths"
	,"NtGdiSTROBJ_dwGetCodePage"
	,"NtGdiSTROBJ_vEnumStart"
	,"NtGdiScaleViewportExtEx"
	,"NtGdiScaleWindowExtEx"
	,"NtGdiSelectBrush"
	,"NtGdiSelectClipPath"
	,"NtGdiSelectPen"
	,"NtGdiSetBitmapAttributes"
	,"NtGdiSetBrushAttributes"
	,"NtGdiSetColorAdjustment"
	,"NtGdiSetColorSpace"
	,"NtGdiSetDeviceGammaRamp"
	,"NtGdiSetFontXform"
	,"NtGdiSetIcmMode"
	,"NtGdiSetLinkedUFIs"
	,"NtGdiSetMagicColors"
	,"NtGdiSetOPMSigningKeyAndSequenceNumbers"
	,"NtGdiSetPUMPDOBJ"
	,"NtGdiSetPixelFormat"
	,"NtGdiSetRectRgn"
	,"NtGdiSetSizeDevice"
	,"NtGdiSetSystemPaletteUse"
	,"NtGdiSetTextJustification"
	,"NtGdiSfmGetNotificationTokens"
	,"NtGdiStartDoc"
	,"NtGdiStartPage"
	,"NtGdiStrokeAndFillPath"
	,"NtGdiStrokePath"
	,"NtGdiSwapBuffers"
	,"NtGdiTransparentBlt"
	,"NtGdiUMPDEngFreeUserMem"
	,"DxgStubAlphaBlt"
	,"EngRestoreFloatingPointState"
	,"NtGdiUpdateColors"
	,"NtGdiUpdateTransform"
	,"NtGdiWidenPath"
	,"NtGdiXFORMOBJ_bApplyXform"
	,"NtGdiXFORMOBJ_iGetXform"
	,"NtGdiXLATEOBJ_cGetPalette"
	,"NtGdiXLATEOBJ_hGetColorTransform"
	,"NtGdiXLATEOBJ_iXlate"
	,"NtUserAddClipboardFormatListener"
	,"NtUserAssociateInputContext"
	,"NtUserBlockInput"
	,"NtUserBuildHimcList"
	,"NtUserBuildPropList"
	,"NtUserCalculatePopupWindowPosition"
	,"NtUserCallHwndOpt"
	,"NtUserChangeDisplaySettings"
	,"NtUserChangeWindowMessageFilterEx"
	,"NtUserCheckAccessForIntegrityLevel"
	,"NtUserCheckDesktopByThreadId"
	,"NtUserCheckWindowThreadDesktop"
	,"NtUserChildWindowFromPointEx"
	,"NtUserClipCursor"
	,"NtUserCreateDesktopEx"
	,"NtUserCreateInputContext"
	,"NtUserCreateWindowStation"
	,"NtUserCtxDisplayIOCtl"
	,"NtUserDestroyInputContext"
	,"NtUserDisableThreadIme"
	,"NtUserDisplayConfigGetDeviceInfo"
	,"NtUserDisplayConfigSetDeviceInfo"
	,"NtUserDoSoundConnect"
	,"NtUserDoSoundDisconnect"
	,"NtUserDragDetect"
	,"NtUserDragObject"
	,"NtUserDrawAnimatedRects"
	,"NtUserDrawCaption"
	,"NtUserDrawCaptionTemp"
	,"NtUserDrawMenuBarTemp"
	,"NtUserDwmStartRedirection"
	,"NtUserDwmStopRedirection"
	,"NtUserEndMenu"
	,"NtUserEndTouchOperation"
	,"NtUserEvent"
	,"NtUserFlashWindowEx"
	,"NtUserFrostCrashedWindow"
	,"NtUserGetAppImeLevel"
	,"NtUserGetCaretPos"
	,"NtUserGetClipCursor"
	,"NtUserGetClipboardViewer"
	,"NtUserGetComboBoxInfo"
	,"NtUserGetCursorInfo"
	,"NtUserGetDisplayConfigBufferSizes"
	,"NtUserGetGestureConfig"
	,"NtUserGetGestureExtArgs"
	,"NtUserGetGestureInfo"
	,"NtUserGetGuiResources"
	,"NtUserGetImeHotKey"
	,"NtUserGetImeInfoEx"
	,"NtUserGetInputLocaleInfo"
	,"NtUserGetInternalWindowPos"
	,"NtUserGetKeyNameText"
	,"NtUserGetKeyboardLayoutName"
	,"NtUserGetLayeredWindowAttributes"
	,"NtUserGetListBoxInfo"
	,"NtUserGetMenuIndex"
	,"NtUserGetMenuItemRect"
	,"NtUserGetMouseMovePointsEx"
	,"NtUserGetPriorityClipboardFormat"
	,"NtUserGetRawInputBuffer"
	,"NtUserGetRawInputData"
	,"NtUserGetRawInputDeviceInfo"
	,"NtUserGetRawInputDeviceList"
	,"NtUserGetRegisteredRawInputDevices"
	,"NtUserGetTopLevelWindow"
	,"NtUserGetTouchInputInfo"
	,"NtUserGetUpdatedClipboardFormats"
	,"NtUserGetWOWClass"
	,"NtUserGetWindowCompositionAttribute"
	,"NtUserGetWindowCompositionInfo"
	,"NtUserGetWindowDisplayAffinity"
	,"NtUserGetWindowMinimizeRect"
	,"NtUserGetWindowRgnEx"
	,"NtUserGhostWindowFromHungWindow"
	,"NtUserHardErrorControl"
	,"NtUserHiliteMenuItem"
	,"NtUserHungWindowFromGhostWindow"
	,"NtUserHwndQueryRedirectionInfo"
	,"NtUserHwndSetRedirectionInfo"
	,"NtUserImpersonateDdeClientWindow"
	,"NtUserInitTask"
	,"NtUserInitialize"
	,"NtUserInitializeClientPfnArrays"
	,"NtUserInjectGesture"
	,"NtUserInternalGetWindowIcon"
	,"NtUserIsTopLevelWindow"
	,"NtUserIsTouchWindow"
	,"NtUserLoadKeyboardLayoutEx"
	,"NtUserLockWindowStation"
	,"NtUserLockWorkStation"
	,"NtUserLogicalToPhysicalPoint"
	,"NtUserMNDragLeave"
	,"NtUserMNDragOver"
	,"NtUserMagControl"
	,"NtUserMagGetContextInformation"
	,"NtUserMagSetContextInformation"
	,"NtUserManageGestureHandlerWindow"
	,"NtUserMenuItemFromPoint"
	,"NtUserMinMaximize"
	,"NtUserModifyWindowTouchCapability"
	,"NtUserNotifyIMEStatus"
	,"NtUserOpenInputDesktop"
	,"NtUserOpenThreadDesktop"
	,"NtUserPaintMonitor"
	,"NtUserPhysicalToLogicalPoint"
	,"NtUserPrintWindow"
	,"NtUserQueryDisplayConfig"
	,"NtUserQueryInformationThread"
	,"NtUserQueryInputContext"
	,"NtUserQuerySendMessage"
	,"NtUserRealChildWindowFromPoint"
	,"NtUserRealWaitMessageEx"
	,"NtUserRegisterErrorReportingDialog"
	,"NtUserRegisterHotKey"
	,"NtUserRegisterRawInputDevices"
	,"NtUserRegisterServicesProcess"
	,"NtUserRegisterSessionPort"
	,"NtUserRegisterTasklist"
	,"NtUserRegisterUserApiHook"
	,"NtUserRemoteConnect"
	,"NtUserRemoteRedrawRectangle"
	,"NtUserRemoteRedrawScreen"
	,"NtUserRemoteStopScreenUpdates"
	,"NtUserRemoveClipboardFormatListener"
	,"NtUserResolveDesktopForWOW"
	,"NtUserSendTouchInput"
	,"NtUserSetAppImeLevel"
	,"NtUserSetChildWindowNoActivate"
	,"NtUserSetClassWord"
	,"NtUserSetCursorContents"
	,"NtUserSetDisplayConfig"
	,"NtUserSetGestureConfig"
	,"NtUserSetImeHotKey"
	,"NtUserSetImeInfoEx"
	,"NtUserSetImeOwnerWindow"
	,"NtUserSetInternalWindowPos"
	,"NtUserSetLayeredWindowAttributes"
	,"NtUserSetMenu"
	,"NtUserSetMenuContextHelpId"
	,"NtUserSetMenuFlagRtoL"
	,"NtUserSetMirrorRendering"
	,"NtUserSetObjectInformation"
	,"NtUserSetProcessDPIAware"
	,"NtUserSetShellWindowEx"
	,"NtUserSetSysColors"
	,"NtUserSetSystemCursor"
	,"NtUserSetSystemTimer"
	,"NtUserSetThreadLayoutHandles"
	,"NtUserSetWindowCompositionAttribute"
	,"NtUserSetWindowDisplayAffinity"
	,"NtUserSetWindowRgnEx"
	,"NtUserSetWindowStationUser"
	,"NtUserSfmDestroyLogicalSurfaceBinding"
	,"NtUserSfmDxBindSwapChain"
	,"NtUserSfmDxGetSwapChainStats"
	,"NtUserSfmDxOpenSwapChain"
	,"NtUserSfmDxQuerySwapChainBindingStatus"
	,"NtUserSfmDxReleaseSwapChain"
	,"NtUserSfmDxReportPendingBindingsToDwm"
	,"NtUserSfmDxSetSwapChainBindingStatus"
	,"NtUserSfmDxSetSwapChainStats"
	,"NtUserSfmGetLogicalSurfaceBinding"
	,"NtUserShowSystemCursor"
	,"NtUserSoundSentry"
	,"NtUserSwitchDesktop"
	,"NtUserTestForInteractiveUser"
	,"NtUserTrackPopupMenuEx"
	,"NtUserUnloadKeyboardLayout"
	,"NtUserUnlockWindowStation"
	,"NtUserUnregisterHotKey"
	,"NtUserUnregisterSessionPort"
	,"NtUserUnregisterUserApiHook"
	,"NtUserUpdateInputContext"
	,"NtUserUpdateInstance"
	,"NtUserUpdateLayeredWindow"
	,"NtUserUpdatePerUserSystemParameters"
	,"NtUserUpdateWindowTransform"
	,"NtUserUserHandleGrantAccess"
	,"NtUserValidateHandleSecure"
	,"NtUserWaitForInputIdle"
	,"NtUserWaitForMsgAndEvent"
	,"NtUserWindowFromPhysicalPoint"
	,"NtUserYieldTask"
	,"NtUserSetClassLongPtr"
	,"NtUserSetWindowLongPtr"
};
// CDlgSSSDT 对话框

IMPLEMENT_DYNAMIC(CDlgSSSDT, CPropertyPage)

CDlgSSSDT::CDlgSSSDT()
	: CPropertyPage(CDlgSSSDT::IDD)
{

}

CDlgSSSDT::~CDlgSSSDT()
{
}

void CDlgSSSDT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_sssdt);
}


BEGIN_MESSAGE_MAP(CDlgSSSDT, CPropertyPage)
END_MESSAGE_MAP()


// CDlgSSSDT 消息处理程序


BOOL CDlgSSSDT::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_list_sssdt.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_list_sssdt.InsertColumn(0, L"index", 0, 50);
	m_list_sssdt.InsertColumn(1, L"函数名称", 0, 150);
	m_list_sssdt.InsertColumn(2, L"当前函数地址", LVCFMT_CENTER, 130);
	m_list_sssdt.InsertColumn(3, L"hook", LVCFMT_CENTER, 70);
	m_list_sssdt.InsertColumn(4, L"原始函数地址", 0, 130);
	m_list_sssdt.InsertColumn(5, L"当前地址所在模块", LVCFMT_CENTER, 200); 

	EnumSssdt();
	return TRUE;
}


void CDlgSSSDT::EnumSssdt()
{
	CString str;
	ULONG64	funcIndex = 0x1000;
	SSSDT_INFO sssdt_info = { 0 };
	DWORD nItem = 0;
	WCHAR	szSysRootBuff[MAX_PATH] = { 0 };
	CString csSysroot;
	GetWindowsDirectoryW(szSysRootBuff, MAX_PATH);
	csSysroot.Format(L"%s", szSysRootBuff);

	memset(&sssdt_info, 0, sizeof(SSSDT_INFO));

	
	// win7 x64  sssdt 827个
	for ( int i = 0; i < 827; i++)
	{
		funcIndex = i;
		if (!ARKDeviceIoControl(IOCTL_GetSssdtFuncAddr, &funcIndex, sizeof(ULONG64), &sssdt_info, sizeof(SSSDT_INFO)))
			continue;
		nItem = m_list_sssdt.GetItemCount();

		//index 
		str.Format(L"%d", i);
		m_list_sssdt.InsertItem(nItem, str);

		// 函数名称
		str.Format(L"%S", SssdtFunWin7x64[i]);
		m_list_sssdt.SetItemText(nItem, 1, str);

		// 当前地址

		str.Format(L"0x%p", sssdt_info.Address);
		m_list_sssdt.SetItemText(nItem, 2, str);

		// hook

		str.Format(L"-");
		m_list_sssdt.SetItemText(nItem, 3, str);

		// orgAddress
		str.Format(L"0xFFFFFFFFFFFFFFF");
		m_list_sssdt.SetItemText(nItem, 4, str);

		// 模块
		str.Format(L"%S", sssdt_info.ImgPath);
		str.Replace(L"\\SystemRoot", csSysroot);
		m_list_sssdt.SetItemText(nItem, 5, str);

		memset(&sssdt_info, 0, sizeof(SSSDT_INFO));
	}


	GetSssdtOrgAddress();
}


BOOLEAN CDlgSSSDT::GetSssdtOrgAddress()
{
	CString str;
	CString szCurrAddr;
	ULONG64 W32pServiceTable, Win32kBase, Win32kImageBase, Win32kInProcess = 0, retv;

	if (m_list_sssdt.GetItemCount() < 826)
		return FALSE;

	if (!ARKDeviceIoControl(IOCTL_GetW32pServiceTable, NULL, 0, &W32pServiceTable, 8))
		return FALSE;
	
	Win32kBase = GetWin32kBase();
	CopyFileA("c:\\windows\\system32\\win32k.sys", "c:\\win32k.dll", 0);
	Win32kImageBase = GetWin32kImageBase();
	//printf("W32pServiceTable:      %llx\n", W32pServiceTable);
	//printf("WIN32K.SYS base:       %llx\n", Win32kBase);
	//printf("WIN32K.SYS image base: %llx\n\n\n", Win32kImageBase);
	ULONG index = 0;
	if (Win32kInProcess == 0)
		Win32kInProcess = (ULONGLONG)LoadLibraryExA("c:\\win32k.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
	for (index = 0; index < 827; index++)	
	{
		ULONGLONG RVA = W32pServiceTable - Win32kBase;
		ULONGLONG temp = *(PULONGLONG)(Win32kInProcess + RVA + 8 * (ULONGLONG)index);
		ULONGLONG RVA_index = temp - Win32kImageBase;
		retv = RVA_index + Win32kBase;
		//printf("Shadow SSDT Function[%ld]: %llx\n", index, retv);
		str.Format(L"0x%p",retv);
		m_list_sssdt.SetItemText(index, 4, str);
		szCurrAddr = m_list_sssdt.GetItemText(index, 2);

		if (str != szCurrAddr)
		{
			m_list_sssdt.SetItemText(index, 3, L"!!!!!!!!!!!");
		}
	}

	return TRUE;
}


ULONG64 CDlgSSSDT::GetWin32kBase()
{
	ULONG NeedSize, i, ModuleCount, BufferSize = 0x5000;
	PVOID pBuffer = NULL;
	ULONGLONG qwBase = 0;
	NTSTATUS Result;
	PSYSTEM_MODULE_INFORMATION pSystemModuleInformation;
	if (ZwQuerySystemInformation == NULL)
		ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(LoadLibraryA("ntdll.dll"), "ZwQuerySystemInformation");
	do
	{
		//分配内存
		pBuffer = malloc(BufferSize);
		if (pBuffer == NULL)
		{
			//printf("malloc Error\n");
			return FALSE;
		}
		//查询模块信息
		Result = ZwQuerySystemInformation(SystemModuleInformation, pBuffer, BufferSize, &NeedSize);
		if (Result == STATUS_INFO_LENGTH_MISMATCH)
		{
			free(pBuffer);
			BufferSize *= 2;
		}
		else if (!NT_SUCCESS(Result))
		{
			//查询失败则退出
			//printf( "ZwQueryInformation Error,the error code:%8X\n", Result );
			free(pBuffer);
			return FALSE;
		}
	} while (Result == STATUS_INFO_LENGTH_MISMATCH);
	pSystemModuleInformation = (PSYSTEM_MODULE_INFORMATION)pBuffer;
	//获得模块的总数量
	ModuleCount = pSystemModuleInformation->Count;
	//遍历所有的模块
	for (i = 0; i < ModuleCount; i++)
	{
		//获得模块的路径
		/*drv_path=KmGetDrvPath(Replace(pSystemModuleInformation->Module[i].ImageName,"\\??\\","",1,1,1));
		if(OnlyShowNoSign==0) //显示所有模块
		{
		if((ULONG64)(pSystemModuleInformation->Module[i].Base) > (ULONG64)0x8000000000000000)
		printf("%0.4ld\t0x%0.16llx\t%s\n",i,pSystemModuleInformation->Module[i].Base,AddTab(drv_path,48,4)); //+pSystemModuleInformation->Module[i].ModuleNameOffset
		}
		else //仅仅显示不带签名的模块
		{
		if(CheckFileTrust(drv_path)==FALSE && (ULONG64)(pSystemModuleInformation->Module[i].Base) > (ULONG64)0x8000000000000000)
		printf("%0.4ld\t0x%0.16llx\t%s\n",i,pSystemModuleInformation->Module[i].Base,AddTab(drv_path,48,4));
		}*/
		if (_stricmp(pSystemModuleInformation->Module[i].ImageName + pSystemModuleInformation->Module[i].ModuleNameOffset, "win32k.sys") == 0)
		{
			qwBase = (ULONGLONG)pSystemModuleInformation->Module[i].Base;
			break;
		}
	}
	/*if(ModuleName!=NULL)
	strcpy(ModuleName,pSystemModuleInformation->Module[0].ImageName+pSystemModuleInformation->Module[0].ModuleNameOffset);
	qwBase=(ULONGLONG)pSystemModuleInformation->Module[0].Base;*/
	free(pBuffer);
	return qwBase;
}
