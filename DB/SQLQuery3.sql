USE [House1]
GO

/****** Object:  Table [dbo].[tblChamberDisplay]    Script Date: 2/12/2016 9:47:29 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[tblChamberDisplay](
 [id] [int] NOT NULL,
 [strLine1_] [nvarchar](50) NULL,
 [strLine2_] [nvarchar](50) NULL,
 [strLine3_] [nvarchar](50) NULL,
 [strLine4_] [nvarchar](50) NULL,
 [strLine5_] [nvarchar](50) NULL,
 [strLine6_] [nvarchar](50) NULL,
 [strLine7_] [nvarchar](50) NULL,
 [dtUpdate] [datetime] NULL DEFAULT GETDATE(),
 CONSTRAINT [PK_tblChamberDisplay] PRIMARY KEY CLUSTERED
(
 [id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO


