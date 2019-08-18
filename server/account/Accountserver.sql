SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[act_album]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[act_album](
	[act_name] [nchar](10) NOT NULL,
	[SessionKey] [varchar](70) NOT NULL,
	[act_id] [int] NOT NULL,
	[create_time] [datetime] NOT NULL,
	[update_time] [datetime] NOT NULL,
 CONSTRAINT [PK_act_album] PRIMARY KEY CLUSTERED 
(
	[act_name] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Vip_log]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[Vip_log](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[order] [nchar](30) NULL,
	[current_time] [nchar](30) NULL,
	[account] [nchar](30) NULL,
	[month] [int] NULL,
	[credit] [int] NULL,
	[vip_type] [int] NULL,
	[ip] [nchar](30) NULL,
	[server_info] [nchar](30) NULL,
	[memo] [nchar](30) NULL,
	[create_time] [datetime] NULL,
 CONSTRAINT [PK_vip_log] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Trade_log]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[Trade_log](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[order] [nchar](32) NULL,
	[current_time] [nchar](30) NULL,
	[account] [nchar](30) NULL,
	[character] [nchar](30) NULL,
	[ip] [nchar](30) NULL,
	[credit] [int] NULL,
	[rate] [int] NULL,
	[server_info] [nchar](30) NULL,
	[memo] [nchar](30) NULL,
	[create_time] [datetime] NULL,
 CONSTRAINT [PK_trade_log] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[RecordTrade_log]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[RecordTrade_log](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[order] [nchar](30) NULL,
	[account] [nchar](30) NULL,
	[character] [nchar](30) NULL,
	[ip] [nchar](30) NULL,
	[store_id] [int] NULL,
	[store_name] [nchar](30) NULL,
	[store_class] [int] NULL,
	[current_time] [nchar](30) NULL,
	[credit] [int] NULL,
	[number] [int] NULL,
	[server_info] [nchar](30) NULL,
	[result] [nchar](30) NULL,
	[create_time] [datetime] NULL,
 CONSTRAINT [PK_trade_record_log] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[GameDBServerList]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[GameDBServerList](
	[server_section] [int] NOT NULL,
	[server_ip] [nvarchar](128) NOT NULL,
	[server_uid] [nvarchar](128) NOT NULL,
	[server_pwd] [nvarchar](128) NOT NULL,
	[server_db] [nvarchar](128) NOT NULL
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[LogRandomAccount]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[LogRandomAccount](
	[accountName] [varchar](50) NOT NULL,
	[addDate] [datetime] NULL,
	[expireDate] [datetime] NULL,
	[ip] [varchar](15) NULL,
	[plainPassword] [varchar](32) NULL,
 CONSTRAINT [PK_LogRandomAccount] PRIMARY KEY CLUSTERED 
(
	[accountName] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[account_charge]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[account_charge](
	[id] [int] NOT NULL,
	[charge_flag] [int] NULL,
	[charge_begin_tick] [int] NULL,
	[charge_end_tick] [int] NULL,
	[saves] [int] NULL,
 CONSTRAINT [PK_account_charge] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[account_login_platform]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[account_login_platform](
	[id] [int] NOT NULL,
	[name] [nvarchar](50) NULL,
	[login_status] [int] NULL,
	[last_login_time] [datetime] NULL,
	[last_logout_time] [datetime] NULL,
	[ban] [int] NULL,
 CONSTRAINT [PK_account_login_platform] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[safety]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[safety](
	[aid] [bigint] IDENTITY(1,1) NOT NULL,
	[allowIP] [varchar](32) NOT NULL
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[user_log]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[user_log](
	[log_id] [int] IDENTITY(1,1) NOT NULL,
	[user_id] [int] NOT NULL,
	[user_name] [varchar](50) NOT NULL,
	[login_time] [datetime] NULL,
	[logout_time] [datetime] NULL,
	[login_ip] [varchar](20) NULL,
 CONSTRAINT [PK_user_log] PRIMARY KEY CLUSTERED 
(
	[log_id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[StrPadLeft]') AND type in (N'FN', N'IF', N'TF', N'FS', N'FT'))
BEGIN
execute dbo.sp_executesql @statement = N'





/*
 * 2005-08-03 12:55
 * Joe
 */
CREATE  FUNCTION [dbo].[StrPadLeft] (@str varchar(8000), @totalWidth int, @paddingChar char(1))
RETURNS varchar(8000) AS
BEGIN
	while len(@str) < @totalWidth
	begin
		set @str = @paddingChar + @str;
	end

	return @str;
END












' 
END

GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[account_login]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[account_login](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[name] [varchar](50) NULL,
	[password] [varchar](50) NULL,
	[originalPassword] [varchar](50) NULL,
	[sid] [int] NOT NULL CONSTRAINT [DF_account_login_sid]  DEFAULT ((0)),
	[login_status] [int] NULL CONSTRAINT [DF_account_login_login_status]  DEFAULT ((0)),
	[enable_login_tick] [bigint] NULL CONSTRAINT [DF_account_login_enable_login_tick]  DEFAULT ((0)),
	[login_group] [varchar](50) NULL,
	[last_login_time] [datetime] NULL CONSTRAINT [DF_account_login_last_login_time]  DEFAULT (getdate()),
	[last_logout_time] [datetime] NULL CONSTRAINT [DF_account_login_last_logout_time]  DEFAULT (getdate()),
	[last_login_ip] [varchar](50) NULL,
	[enable_login_time] [datetime] NULL CONSTRAINT [DF_account_login_enable_login_time]  DEFAULT ('2001-1-1 1:1:1'),
	[total_live_time] [bigint] NOT NULL CONSTRAINT [DF_account_login_total_live_time]  DEFAULT ((0)),
	[last_login_mac] [varchar](50) NULL,
	[ban] [int] NULL,
 CONSTRAINT [PK_account_login] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY],
 CONSTRAINT [IX_account_login] UNIQUE NONCLUSTERED 
(
	[name] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY],
 CONSTRAINT [IX_account_login_name] UNIQUE NONCLUSTERED 
(
	[name] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[TradeRecord]') AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[TradeRecord](
	[record_id] [nvarchar](50) NOT NULL,
	[record_message] [int] NOT NULL,
	[record_section] [int] NOT NULL,
	[record_info] [nvarchar](4000) NOT NULL,
	[record_time] [datetime] NOT NULL,
	[ret_info] [nvarchar](4000) NOT NULL,
	[ret_time] [datetime] NOT NULL,
	[state] [int] NOT NULL,
	[flow_id] [nvarchar](50) NULL,
	[production_id] [nvarchar](50) NULL,
 CONSTRAINT [IX_TradeRecord] UNIQUE NONCLUSTERED 
(
	[record_id] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
END
GO
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Create_GameAccount]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'


CREATE PROCEDURE [dbo].[Create_GameAccount] 
	@Prefix nvarchar(50)=''KopTest'' ,--用户名前缀
	@SartNo int = 0, --开始序号
	@Number int = 1, --帐号,
	@TempletID int --样版角色ID
AS
BEGIN
	
	SET NOCOUNT ON;
	DECLARE @AccountID int --帐号ID
	DECLARE @CharacterID int --角色ID
	DECLARE @UserName nvarchar(50) --用户名称
	DECLARE @CharacterName nvarchar(50) --角色名称
	DECLARE @CharacterList nvarchar(100) --角色ID列表
	DECLARE @Password nvarchar(50) --用户密码
	DECLARE @KitBagID int --临时背包ID
	DECLARE @KitBagIDTemp int --临时背包ID
	DECLARE @BankID int --银行ID
	DECLARE @MapMaskID int
	DECLARE @No int --序号
	

	BEGIN TRAN CREATEACCOUNTTRAN

	SET @Password = ''96E79218965EB72C92A549DD5A330112'' --111111
	SET @No = 0
	WHILE (@No < @Number)
	BEGIN
		SET @CharacterList = ''0''
		SET @UserName = @Prefix + '''' + CAST( (@SartNo + @No) as nvarchar)
		--创建帐号
		INSERT INTO AccountServer.dbo.account_login(name,password,originalPassword)
			VALUES(@UserName,@Password,@Password);		
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK
		SELECT @AccountID = @@IDENTITY
		--PRINT @AccountID

		--创建角色
		INSERT INTO GameDB.dbo.account(act_id,act_name,gm,cha_ids,password)
			VALUES(@AccountID,@UserName,0,''0'',@Password)
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK
		
		
		INSERT INTO GameDB.dbo.character(act_id,cha_name,bank)
			VALUES(@AccountID,@UserName,'''')
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK
		SELECT @CharacterID = @@IDENTITY	
		SET @CharacterList  = CAST( @CharacterID as nvarchar) + '';''
		--PRINT @CharacterList

		--背包ID
		INSERT INTO GameDB.dbo.Resource(cha_id,type_id,content)
		SELECT @CharacterID,1,content
		FROM GameDB.dbo.Resource 
		WHERE cha_id=@TempletID AND type_id=1
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK			
		SELECT @KitBagID = @@IDENTITY
		PRINT @KitBagID

		--银行ID
		INSERT INTO GameDB.dbo.Resource(cha_id,type_id,content)
		SELECT @CharacterID,2,content
		FROM GameDB.dbo.Resource 
		WHERE cha_id=@TempletID AND type_id=2
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK
		SELECT @BankID = @@IDENTITY
		
		--临时背包ID
		INSERT INTO GameDB.dbo.Resource(cha_id,type_id,content)
		SELECT @CharacterID,3,content
		FROM GameDB.dbo.Resource 
		WHERE cha_id=@TempletID AND type_id=3
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK
		SELECT @KitBagIDTemp = @@IDENTITY
			
		--
		INSERT INTO GameDB.dbo.map_mask(cha_id,content1,content2,content3,content4,content5)
			VALUES(@CharacterID,''0'',''0'',''0'',''0'',''0'')
		SELECT @MapMaskID = @@IDENTITY
		UPDATE GameDB.dbo.account SET cha_ids = @CharacterList 
			WHERE act_id =@AccountID
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK

		--更新角色信息
	
		UPDATE GameDB.dbo.character SET 
			kitbag=@KitBagID,
			bank = @BankID,
			kitbag_tmp = @KitBagIDTemp,
			map_mask = @MapMaskID,
			icon = A.A_icon,
			version = A.A_version,
			hp = A.A_hp,
			sp = A.A_sp,
			ap = A.A_ap,
			[str] = A.A_str,
			dex = A.A_dex,
			agi = A.A_agi,
			con = A.A_con,
			sta = A.A_sta,
			luk = A.A_luk,
			sail_lv = A.A_sail_lv,
			live_lv = A.A_live_lv,
			job = A.A_job,
			degree = A.A_degree,
			map = A.A_map,
			map_x = A.A_map_x,
			map_y = A.A_map_y,
			radius = A.A_radius,
			angle = A.A_angle,
			look = A.A_look,
			kb_capacity = A.A_kb_capacity,
			skillbag = A.A_skillbag,
			shortcut = A.A_shortcut,
			mission = A.A_mission,
			misrecord = A.A_misrecord,
			mistrigger = A.A_mistrigger,
			miscount = A.A_miscount,
			birth = A.A_birth,
			login_cha = A.A_login_cha,	
			live_tp = A.A_live_tp,					
			main_map = A.A_main_map,
			skill_state = A.A_skill_state,
			estop = A.A_estop,
			estoptime = A.A_estoptime,
			kb_locked = A.A_kb_locked,
			credit = A.A_credit,
			store_item = A.A_store_item,
			extend = A.A_extend
		FROM (
			SELECT 
				motto ,
				icon AS A_icon,
				version as A_version,
				pk_ctrl as A_pk_ctrl,
				job as A_job,
				degree as A_degree,
				[exp] as A_exp,
				hp as A_hp,
				sp as A_sp,
				ap as A_ap,
				tp as A_tp,
				gd as A_gd,
				[str] as A_str,
				dex as A_dex,
				agi as A_agi,
				con as A_con,
				sta as A_sta,
				luk as A_luk,
				sail_lv as A_sail_lv,
				sail_exp as A_sail_exp,
				sail_left_exp as A_sail_left_exp,
				live_lv as A_live_lv,
				live_exp as A_live_exp,
				map as A_map,
				map_x as A_map_x,
				map_y as A_map_y,
				radius as A_radius,
				angle as A_angle,
				look as A_look,
				kb_capacity as A_kb_capacity,
				kitbag as A_kitbag,
				skillbag as A_skillbag,
				shortcut as A_shortcut,
				mission as A_mission,
				misrecord as A_misrecord,
				mistrigger as A_mistrigger,
				miscount as A_miscount,
				birth as A_birth,
				login_cha as A_login_cha,
				live_tp as A_live_tp,
				map_mask as A_map_mask,
				main_map as A_main_map,
				skill_state as A_skill_state,
				estop as A_estop,
				estoptime as A_estoptime,
				kb_locked as A_kb_locked,
				credit as A_credit,
				store_item as A_store_item,
				extend as A_extend
			FROM GameDB.dbo.character
			WHERE cha_id = @TempletID 
			) AS A
		WHERE cha_id = @CharacterID			
		IF (@@ERROR <> 0 ) GOTO ERROR_ROLLBACK

		SET @No = @No + 1
	END
	
	COMMIT TRAN CREATEACCOUNTTRAN
	PRINT ''生成完成！''
	GOTO EXIT_TRAN
	
	ERROR_ROLLBACK:
		ROLLBACK TRAN CREATEACCOUNTTRAN
		PRINT ''生成失败!''
	
	EXIT_TRAN:
	
	
	SET NOCOUNT OFF;
END


' 
END
