#ifndef __DSOUNDMANAGER_H__
#define __DSOUNDMANAGER_H__

#include "SoundManager.h"

class DSoundInstance;

class DSoundManager : public SoundManager
{
	friend class DSoundInstance;
	friend class DSoundMusicInterface;

protected:
	HMODULE					mDSoundDLL;
	LPDIRECTSOUNDBUFFER		mSourceSounds[MAX_SOURCE_SOUNDS];
	string					mSourceFileNames[MAX_SOURCE_SOUNDS];
	LPDIRECTSOUNDBUFFER		mPrimaryBuffer;
	ulong					mSourceDataSizes[MAX_SOURCE_SOUNDS];
	double					mBaseVolumes[MAX_SOURCE_SOUNDS];
	int						mBasePans[MAX_SOURCE_SOUNDS];
	DSoundInstance*			mPlayingSounds[MAX_CHANNELS];	
	double					mMasterVolume;
	DWORD					mLastReleaseTick;
	float					m_fFadeByWindow;

protected:
	int						FindFreeChannel();
	int						VolumeToDB(double theVolume);
	bool					LoadOGGSound(unsigned int theSfxID, const string& theFilename);
	bool					LoadWAVSound(unsigned int theSfxID, const string& theFilename);
	bool					LoadAUSound(unsigned int theSfxID, const string& theFilename);
	bool					WriteWAV(unsigned int theSfxID, const string& theFilename, const string& theDepFile);
	bool					GetTheFileTime(const string& theDepFile, FILETIME* theFileTime);
	void					ReleaseFreeChannels();

public:
	LPDIRECTSOUND			mDirectSound;

	DSoundManager(HWND theHWnd);
	virtual ~DSoundManager();

	virtual bool			Initialized();

	virtual bool			LoadSound(int theSfxID, const string& theFilename);
	virtual int				LoadSound(const string& theFilename);
	virtual void			ReleaseSound(unsigned int theSfxID);	

	virtual void			SetVolume(double theVolume);
	virtual bool			SetBaseVolume(unsigned int theSfxID, double theBaseVolume);
	virtual bool			SetBasePan(unsigned int theSfxID, int theBasePan);

	virtual SoundInstance*	GetSoundInstance(unsigned int theSfxID);

	virtual void			ReleaseSounds();
	virtual void			ReleaseChannels();		

	virtual double			GetMasterVolume();
	virtual void			SetMasterVolume(double theVolume);

	virtual void			Flush();

	virtual void			SetCooperativeWindow(HWND theHWnd, bool isWindowed);
	virtual void			StopAllSounds();

	/*--------------------------------------------------------------------------
	Ӧ�ó����л�������ʱ���������ʵ����ͣ�ʹ��������� by Waiting 2009-06-22
	1.0f ��������		Ӧ�ó�����ǰ��
	0.0f ����			������С,����ȫ������Ӧ�ó����ڵ�
	0.5f һ�������		Ӧ�ó���¶����һ����
	--------------------------------------------------------------------------*/
	virtual void			FadeByWindow(float fFade);
};

#endif //__DSOUNDMANAGER_H__