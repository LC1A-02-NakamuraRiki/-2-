#include <Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include <vector>
#include <string>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>


#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include<d3dx12.h>
#include<DirectXTex.h>
#include<wrl.h>
using namespace Microsoft::WRL;

#define _RPT0(rptno, msg)      _RPTN(rptno, "%s", msg)

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"


struct ConstBufferData
{
	XMFLOAT4 color;    //色(RGBA)
	XMMATRIX mat;      //3D変換行列
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
};

struct PipelineSet
{
	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelinestate;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;
};

struct Object3d
{
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//定数バッファビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
	//定数バッファビューのハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	//アフィン変換情報
	XMFLOAT3 scale = { 1, 1, 1 };     //スケーリング倍率
	XMFLOAT3 rotation = { 0, 0, 0 };  //回転角
	XMFLOAT3 position = { 0, 0, 0 };  //座標
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d *parent = nullptr;
};

struct VertexPosUv
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct Sprite
{

	ComPtr<ID3D12Resource> vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ComPtr<ID3D12Resource> constBuff;

	float rotation = 0.0f;

	XMFLOAT3 position = { 0, 0, 0 };

	XMMATRIX matWorld;

	XMFLOAT4 color = { 1, 1, 1, 1 };

	UINT texNumber = 0;

	XMFLOAT2 size = { 100,100 };

	XMFLOAT2 anchorpoint = { 0.5f, 0.5f };

	bool isFlipX = false;

	bool isFlipY = false;

	XMFLOAT2 texBase = { 0, 0 };

	XMFLOAT2 texSize = { 560, 560 };

	bool isInvisible = false;
};

const int spriteSRVCount = 512;

struct SpriteCommon
{
	PipelineSet pipelineSet;

	XMMATRIX matProjection{};

	ComPtr<ID3D12DescriptorHeap> descHeap;

	ComPtr<ID3D12Resource> texBuff[spriteSRVCount];
};

class DebugText
{
public:

	static const int maxCharCount = 256;
	static const int fontWidth = 18;
	static const int fontHeight = 36;
	static const int fontLineCount = 16;

	Sprite sprites[maxCharCount];

	int spriteIndex = 0;

	void Initialize(ID3D12Device *dev, int window_width, int window_height, UINT texNumber,
		SpriteCommon &spriteCommon);
	void Print(const SpriteCommon &spriteCommon, const std::string &text, float x, float y, float scale);
	void DrawAll(ID3D12GraphicsCommandList *cmdList, const SpriteCommon &spriteCommon, ID3D12Device *dev);

private:


};

struct ChunkHeader
{
	char id[4];
	int32_t size;
};

struct RiffHeader
{
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk
{
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

struct SoundData
{
	WAVEFORMATEX wfex;

	BYTE *pBuffer;

	unsigned int bufferSize;
};
void InitializeObject3d(Object3d *object, int index, ID3D12Device *dev,
	ComPtr<ID3D12DescriptorHeap> descHeap);
void UpdateObject3d(Object3d *object, XMMATRIX &matView, XMMATRIX &matProjection);
void DrawObject3d(Object3d *object, ComPtr<ID3D12GraphicsCommandList> cmdList,
	ComPtr<ID3D12DescriptorHeap> descHeap, D3D12_VERTEX_BUFFER_VIEW &vbView,
	D3D12_INDEX_BUFFER_VIEW &ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
	UINT numIndices);
PipelineSet Object3dCreateGraphicsPipeline(ID3D12Device *dev);
PipelineSet spriteCreateGraphicsPipeline(ID3D12Device *dev);
Sprite SpriteCreate(ID3D12Device *dev, int window_width, int window_height, UINT texNumber, const SpriteCommon &
	spriteCommon, XMFLOAT2 anchorpoint,
	bool isFlipX, bool isFlipY);
void SpriteCommonBeginDraw(const SpriteCommon &spriteCommon, ID3D12GraphicsCommandList *cmdList);
void SpriteDraw(const Sprite &sprite, ID3D12GraphicsCommandList *cmdList, const SpriteCommon &spriteCommon,
	ID3D12Device *dev);
SpriteCommon SpriteCommonCreate(ID3D12Device *dev, int window_width, int window_height);
void SpriteUpdate(Sprite &sprite, const SpriteCommon &spriteCommon);
void SpriteCommonLoadTexture(SpriteCommon &spriteCommon,
	UINT texnumber, const wchar_t *filename, ID3D12Device *dev);
void SpriteTransferVertexBuffer(const Sprite &sprite, const SpriteCommon &spriteCommon);
SoundData SoundLoadWave(const char *filename);
void SoundUnload(SoundData *soundData);
void SoundPlayWave(IXAudio2 *xAudio2, const SoundData &soundData);

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// メッセージで分岐
	switch (msg) {
	case WM_DESTROY: // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp *winApp = nullptr;

	winApp = new WinApp();
	winApp->Initialize();

	MSG msg{};  // メッセージ
	//WindowsAPI初期化

// DirectX初期化処理　ここから

#ifdef _DEBUG

#endif


	//ポインター置き場
	Input *input = nullptr;
	DirectXCommon *dxCommon = nullptr;

	HRESULT result;
	/*ComPtr<ID3D12Device> dev;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;*/

	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);






	/*dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1 **)&swapchain);*/



		/*D3D12_RESOURCE_DESC depthResDesc{};
		depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResDesc.Width = window_width;
		depthResDesc.Height = window_height;
		depthResDesc.DepthOrArraySize = 1;
		depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthResDesc.SampleDesc.Count = 1;
		depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;*/

		//D3D12_HEAP_PROPERTIES depthHeapProp{};
		//depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

		/*D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

		ComPtr <ID3D12Resource> depthBuffer;
		result = dev->CreateCommittedResource(
			&depthHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&depthBuffer)
		);*/









		//---------------------DirectX初期化処理　ここまで--------------------------//
#pragma endregion Region_2 directXの初期化

	//描画処理///////////////////////////////////////////////////////////////

	//const int texWidth = 256;//横方向ピクセル数
	//const int imageDateCount = texWidth * texWidth;//配列の要素数
	////画像イメージデータ配列
	//XMFLOAT4 *imageDate = new XMFLOAT4[imageDateCount];

	////全ピクセルの色を初期化
	//for (int i = 0; i < imageDateCount; i++)
	//{
	//	imageDate[i].x = 1.0f;//R
	//	imageDate[i].y = 0.0f;//G
	//	imageDate[i].z = 0.0f;//B
	//	imageDate[i].w = 1.0f;//A
	//}







	Vertex vertices[] =
	{
		//前
		{{ -25.0f, -25.0f, -25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -25.0f,  25.0f, -25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  25.0f, -25.0f, -25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  25.0f,  25.0f, -25.0f}, {}, {1.0f, 0.0f}}, // 右上
		//後
		{{  25.0f, -25.0f,  25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  25.0f,  25.0f,  25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -25.0f, -25.0f,  25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -25.0f,  25.0f,  25.0f}, {}, {1.0f, 0.0f}}, // 右上
		//左
		{{ -25.0f, -25.0f, -25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -25.0f, -25.0f,  25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -25.0f,  25.0f, -25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -25.0f,  25.0f,  25.0f}, {}, {1.0f, 0.0f}}, // 右上
		//右
		{{  25.0f,  25.0f, -25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  25.0f,  25.0f,  25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  25.0f, -25.0f, -25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  25.0f, -25.0f,  25.0f}, {}, {1.0f, 0.0f}}, // 右上
		//下
		{{  25.0f, -25.0f, -25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  25.0f, -25.0f,  25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -25.0f, -25.0f, -25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -25.0f, -25.0f,  25.0f}, {}, {1.0f, 0.0f}}, // 右上
		//上
		{{ -25.0f,  25.0f, -25.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -25.0f,  25.0f,  25.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  25.0f,  25.0f, -25.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  25.0f,  25.0f,  25.0f}, {}, {1.0f, 0.0f}}, // 右上
	};

	unsigned short indices[] = {
	0,1,2,
	2,1,3,

	4,5,6,
	6,5,7,

	8,9,10,
	10,9,11,

	12,13,14,
	14,13,15,

	16,17,18,
	18,17,19,

	20,21,22,
	22,21,23,
	};

	for (int i = 0; i < _countof(indices) / 3; i++)
	{//三角形1つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトルp0→p2ベクトルを計算（ベクトルの減算）
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化（長さを1にする）
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	//UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));
	// 頂点バッファの設定
	//D3D12_HEAP_PROPERTIES heapprop{};   // ヒープ設定
	//heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	//D3D12_RESOURCE_DESC resdesc{};  // リソース設定
	//resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//resdesc.Width = sizeVB; // 頂点データ全体のサイズ
	//resdesc.Height = 1;
	//resdesc.DepthOrArraySize = 1;
	//resdesc.MipLevels = 1;
	//resdesc.SampleDesc.Count = 1;
	//resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;



	//// 頂点バッファの生成
	//ComPtr < ID3D12Resource >vertBuff;
	//result = dev->CreateCommittedResource(
	//	&heapprop, // ヒープ設定
	//	D3D12_HEAP_FLAG_NONE,
	//	&resdesc, // リソース設定
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&vertBuff));

	// 頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// GPU上のバッファに対応した仮想メモリを取得
	//XMFLOAT3 *vertMap = nullptr;
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	//vbView.StrideInBytes = sizeof(XMFLOAT3);
	vbView.StrideInBytes = sizeof(Vertex);


	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));
	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//つながりを解除
	indexBuff->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	const int constBufferNum = 128;

	//定数バッファ用のデスクリプタヒープ
	ComPtr <ID3D12DescriptorHeap >basicDescHeap;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = constBufferNum + 1;

	//デスクリプタヒープの生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	//デスクリプタヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
		basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
		basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	//ハンドルのアドレスを進める
	cpuDescHandleSRV.ptr += constBufferNum *
		dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	gpuDescHandleSRV.ptr += constBufferNum *
		dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);




	//関数化してもいい
	const int OBJECT_NUM = 1;
	Object3d object3ds[OBJECT_NUM];
	//配列内の全オブジェクトに対して
	for (int i = 0; i < _countof(object3ds); i++)
	{
		//初期化
		InitializeObject3d(&object3ds[i], i, dxCommon->GetDevice(), basicDescHeap.Get());
		//ここから↓は親子構造体のサンプル
		//先頭以外なら
		if (i > 0)
		{
			//一つ前のオブジェクトを親のオブジェクトとする
			object3ds[i].parent = &object3ds[i - 1];
			//親オブジェクトの9割の大きさ
			object3ds[i].scale = { 0.9f, 0.9f, 0.9f };
			//親オブジェクトに対してZ軸まわりに30度回転
			object3ds[i].rotation = { 0.0f, 0.0f, 30.0f };
			//親オブジェクトに対してZ方向-8.0ずらす
			object3ds[i].position = { 0.0f, 0.0f, -8.0f };
		}
	}



	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		(float)WinApp::window_width / WinApp::window_height,
		0.1f, 1000.0f
	);

	for (int i = 0; i < _countof(object3ds); i++)
	{
		UpdateObject3d(&object3ds[i], matView, matProjection);
	}

	TexMetadata metadate{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/valo.jpg",
		WIC_FLAGS_NONE,
		&metadate, scratchImg);

	const Image *img = scratchImg.GetImage(0, 0, 0);

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadate.format,
		metadate.width,
		(UINT)metadate.height,
		(UINT16)metadate.arraySize,
		(UINT16)metadate.mipLevels
	);

	//テスクチャ用バッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource
	(  //GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  //テスクチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);

	//テクスチャバッファにデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		(UINT)img->rowPitch,//1ラインサイズ
		(UINT)img->slicePitch//全サイズ
	);

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = metadate.format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープの2番目にシェーダリソースビュー作成
	dxCommon->GetDevice()->CreateShaderResourceView(texbuff.Get(),//ビューと関連 
		&srvDesc,//テクスチャ設定情報
		cpuDescHandleSRV
	);

	SpriteCommon spriteCommon;

	spriteCommon = SpriteCommonCreate(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	SpriteCommonLoadTexture(spriteCommon, 0, L"Resources/sakana.jpg", dxCommon->GetDevice());
	SpriteCommonLoadTexture(spriteCommon, 1, L"Resources/sakana.png", dxCommon->GetDevice());

	Sprite sprite;

	sprite = SpriteCreate(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height, 0, spriteCommon, { 0.5f, 0.5f }, false, false);

	/*XMMATRIX matWorld;
	matWorld = XMMatrixIdentity();

	XMMATRIX matScale;
	matScale = XMMatrixScaling(1.0f, 0.5f, 1.0f);
	matWorld *= matScale;

	XMMATRIX matRot;
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(45.0f));
	matRot *= XMMatrixRotationX(XMConvertToRadians(45.0f));
	matRot *= XMMatrixRotationY(XMConvertToRadians(45.0f));
	matWorld *= matRot;

	XMMATRIX matTrans;
	matTrans = XMMatrixTranslation(50.0f, 0, 0);
	matWorld *= matTrans;*/

	//D3D12_HEAP_PROPERTIES texHeapProp{};//テクスチャヒープ設定
	//texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	//texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//D3D12_RESOURCE_DESC texresDesc{};//リソース設定
	//texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadate.dimension);//2Dテクスチャ用
	//texresDesc.Format = metadate.format;//RGBAフォーマット
	//texresDesc.Width = metadate.width;//幅
	//texresDesc.Height = (UINT)metadate.height;//高さ
	//texresDesc.DepthOrArraySize = (UINT16)metadate.arraySize;
	//texresDesc.MipLevels = (UINT16)metadate.mipLevels;
	//texresDesc.SampleDesc.Count = 1;

	//ComPtr < ID3D12Resource >texbuff;
	//result = dev->CreateCommittedResource(//GPUリソース
	//	&texHeapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&texresDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
	//	nullptr,
	//	IID_PPV_ARGS(&texbuff));	

	//元データ解放
	//delete[] imageDate;

	//XMFLOAT3 vertices[] = {
	//{-0.5f,-0.5f,0.0f},// 左下
	//{-0.5f,+0.5f,0.0f},// 左上
	//{+0.5f,-0.5f,0.0f},// 右下
	//{+0.5f,+0.5f,0.0f},//右上
	//};

	//const int DIV = 5;

	//XMFLOAT3 vertices[DIV + 1];

	//for (int i = 0; i < DIV; i++)
	//{
	//	vertices[i].x = 0.5f * sin(2 * XM_PI / DIV * i);
	//	vertices[i].y = 0.5f * cos(2 * XM_PI / DIV * i);
	//	vertices[i].z = 0;
	//}
	//vertices[DIV].x = 0;
	//vertices[DIV].y = 0;
	//vertices[DIV].z = 0;

	//Vertex vertices[] = {
	//	{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
	//	{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
	//	{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
	//	{{100.0f,0.0f,0.0f},{1.0f,0.0f}},
	//};	

	//ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	//ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	//ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	/*unsigned short indices[DIV * 3];

	for (int i = 0; i < DIV; i++)
	{
		indices[i * 3 + 0] = i;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = DIV;
	}
	indices[DIV * 3 - 1] = 0;*/

	//インデックスバッファの設定
	//ComPtr < ID3D12Resource >IndexBuff;
	//resdesc.Width = sizeIB;
	////インデックスバッファの設定
	//result = dev->CreateCommittedResource(
	//	&heapprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&resdesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&IndexBuff));

	XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };     //スケーリング倍率
	XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };  //回転角
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };  //座標

	PipelineSet object3dPipelineSet = Object3dCreateGraphicsPipeline(dxCommon->GetDevice());



	PipelineSet spritePipelineSet = spriteCreateGraphicsPipeline(dxCommon->GetDevice());

	sprite.rotation = 0;
	sprite.position = { 1280 / 2,720 / 2,0 };

	sprite.size.x = 100.0f;
	sprite.size.y = 100.0f;
	SpriteTransferVertexBuffer(sprite, spriteCommon);

	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice *masterVoice;

	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	result = xAudio2->CreateMasteringVoice(&masterVoice);

	//音声読み込み
	SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");

	SoundPlayWave(xAudio2.Get(), soundData1);

	input = new Input();
	input->Initialize(winApp);

	//-----------------------------------------------------------------------------------------------


	float a = 0.0f;
	int moveflag = 0;
	int x = 1.0f;
	int y = 1.0f;
	float angle = 0.0f;

	float PlayerX = -80.0f;
	float PlayerY = 0.0f;
	float PlayerZ = 0.0f;

	SpriteCommon spriteCommon2;

	spriteCommon2 = SpriteCommonCreate(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	DebugText debugText;

	const int debugTextTexNumber = 2;

	SpriteCommonLoadTexture(spriteCommon2, debugTextTexNumber, L"Resources/debugfont.png", dxCommon->GetDevice());

	debugText.Initialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height, debugTextTexNumber, spriteCommon2);
	//-----------------------------------------------------------------------------------------------


	while (true)  // ゲームループ
	{
		// ブロック内はページ右側を参照
		//ウィンドウメッセージ処理　ここから
		if (winApp->ProcessMessage())
		{
			break;
		}
		//if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		//	TranslateMessage(&msg); // キー入力メッセージの処理
		//	DispatchMessage(&msg); // プロシージャにメッセージを送る
		//}

		//// ✖ボタンで終了メッセージが来たらゲームループを抜ける
		//if (msg.message == WM_QUIT) {
		//	break;
		//}
		//ウィンドウメッセージ処理　ここまで
		// DirectX毎フレーム処理　ここから

		input->Update();

		// 更新処理　ここから


		//if (key[DIK_RIGHT] || key[DIK_LEFT])
		//{
		//	if (key[DIK_RIGHT])
		//	{
		//		angle += XMConvertToRadians(1.0f);
		//	}
		//	else if (key[DIK_LEFT])
		//	{
		//		angle -= XMConvertToRadians(1.0f);
		//	}

		//	eye.x = -100 * sinf(angle);
		//	eye.z = -100 * cosf(angle);

		//	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		//	for (int i = 0; i < _countof(object3ds); i++)
		//	{
		//		UpdateObject3d(&object3ds[i], matView, matProjection);
		//	}
		//}


		//キーが押されているときの処理
		if (input->TriggerKey(DIK_0))
		{
			OutputDebugStringA("Hit 0\n");
		}

		//if (key[DIK_W])
		//{
		//	PlayerY += 1.0f;
		//}
		//else if (key[DIK_S])
		//{
		//	PlayerY -= 1.0f;
		//}

		//if (key[DIK_D])
		//{
		//	PlayerX += 1.0f;
		//}
		//else if (key[DIK_A])
		//{
		//	PlayerX -= 1.0f;
		//}

		Vertex vertices[] =
		{
			//前
			{{ -5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{ -5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{  5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{  5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {1.0f, 0.0f}}, // 右上
			//後
			{{  5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{  5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{ -5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{ -5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
			//左
			{{ -5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{ -5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{ -5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{ -5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
			//右
			{{  5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{  5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{  5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{  5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
			//下
			{{  5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{  5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{ -5.0f + PlayerX, -5.0f + PlayerY, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{ -5.0f + PlayerX, -5.0f + PlayerY,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
			//上
			{{ -5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
			{{ -5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
			{{  5.0f + PlayerX,  5.0f + PlayerY, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
			{{  5.0f + PlayerX,  5.0f + PlayerY,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
		};

		for (int i = 0; i < _countof(indices) / 3; i++)
		{//三角形1つごとに計算していく
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short indices0 = indices[i * 3 + 0];
			unsigned short indices1 = indices[i * 3 + 1];
			unsigned short indices2 = indices[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
			//p0→p1ベクトルp0→p2ベクトルを計算（ベクトルの減算）
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積は両方から垂直なベクトル
			XMVECTOR normal = XMVector3Cross(v1, v2);
			//正規化（長さを1にする）
			normal = XMVector3Normalize(normal);
			//求めた法線を頂点データに代入
			XMStoreFloat3(&vertices[indices0].normal, normal);
			XMStoreFloat3(&vertices[indices1].normal, normal);
			XMStoreFloat3(&vertices[indices2].normal, normal);
		}

		// GPU上のバッファに対応した仮想メモリを取得
		Vertex *vertMap = nullptr;
		result = vertBuff->Map(0, nullptr, (void **)&vertMap);

		// 全頂点に対して
		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];   // 座標をコピー
		}

		// マップを解除
		vertBuff->Unmap(0, nullptr);

		//SpriteUpdate(sprite, spriteCommon);
		// 更新処理　ここまで
		// 表示処理　ここから

		// 表示処理　ここまで
		//
		// 

		//デバッグテキスト
		//X座標、Y座標を指定して表示
		//debugText.Print(spriteCommon2, "Hello,DirectX!!", 100, 100, 1.0f);

		//debugText.Print(spriteCommon2, "Nihon Kogakuin", 100, 200, 1.0f);


		

		


		// ４．描画コマンドここから

		_RPTN(_CRT_WARN, "xMove %d : yMove %d\n", (int)PlayerX, (int)PlayerY);

		dxCommon->PreDraw();

		//パイプラインステートとルートシグネチャの設定
		dxCommon->GetCommandList()->SetPipelineState(object3dPipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(object3dPipelineSet.rootsignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(spritePipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(spritePipelineSet.rootsignature.Get());



		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);      //点のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);       //線のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);      //線のストリップ
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   //三角形のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);  //三角形のストリップ


		SpriteCommonBeginDraw(spriteCommon, dxCommon->GetCommandList());

		SpriteDraw(sprite, dxCommon->GetCommandList(), spriteCommon, dxCommon->GetDevice());


		//パイプラインステートとルートシグネチャの設定
		dxCommon->GetCommandList()->SetPipelineState(object3dPipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(object3dPipelineSet.rootsignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(spritePipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(spritePipelineSet.rootsignature.Get());

		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);      //点のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);       //線のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);      //線のストリップ
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   //三角形のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);  //三角形のストリップ


		//デスクリプタヒープの先頭ハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
			basicDescHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
			basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		//ハンドルのアドレスを進める
		cpuDescHandleSRV.ptr += constBufferNum *
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		gpuDescHandleSRV.ptr += constBufferNum *
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], dxCommon->GetCommandList(), basicDescHeap, vbView, ibView, gpuDescHandleSRV, _countof(indices));
		}





		//パイプラインステートとルートシグネチャの設定
		dxCommon->GetCommandList()->SetPipelineState(object3dPipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(object3dPipelineSet.rootsignature.Get());

		dxCommon->GetCommandList()->SetPipelineState(spritePipelineSet.pipelinestate.Get());
		dxCommon->GetCommandList()->SetGraphicsRootSignature(spritePipelineSet.rootsignature.Get());



		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);      //点のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);       //線のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);      //線のストリップ
		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   //三角形のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);  //三角形のストリップ

		SpriteCommonBeginDraw(spriteCommon2, dxCommon->GetCommandList());

		debugText.DrawAll(dxCommon->GetCommandList(), spriteCommon2, dxCommon->GetDevice());


		// ４．描画コマンドここまで

		dxCommon->PostDraw();
		// DirectX毎フレーム処理　ここまで



		// DirectX毎フレーム処理　ここまで

	}

	delete input;


	xAudio2.Reset();
	SoundUnload(&soundData1);
	delete dxCommon;
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;
	return 0;
}



//3Dオブジェクト初期化
void InitializeObject3d(Object3d *object, int index, ID3D12Device *dev,
	ComPtr<ID3D12DescriptorHeap> descHeap)
{
	HRESULT result;

	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuff));

	//デスクリプタ1つ分のサイズ
	UINT descHandleIncrementSize =
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//CBVのCPUデスクリプタハンドルを計算
	object->cpuDescHandleCBV = descHeap->GetCPUDescriptorHandleForHeapStart();
	object->cpuDescHandleCBV.ptr += index * descHandleIncrementSize;
	//CBVのGPUデスクリプタハンドルを計算
	object->gpuDescHandleCBV = descHeap->GetGPUDescriptorHandleForHeapStart();
	object->gpuDescHandleCBV.ptr += index * descHandleIncrementSize;

	//定数バッファビューの生成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = object->constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)object->constBuff->GetDesc().Width;
	dev->CreateConstantBufferView(&cbvDesc, object->cpuDescHandleCBV);
}

//オブジェクト更新処理
void UpdateObject3d(Object3d *object, XMMATRIX &matView, XMMATRIX &matProjection)
{
	//スケーリング行列、回転行列、平行移動行列
	XMMATRIX matScale, matRot, matTrans;
	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(object->rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(object->rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(object->rotation.y));
	matTrans = XMMatrixTranslation(50.0f, 0.0f, 0.0f);  //(50, 0, 0)平行移動

	//ワールド行列
	object->matWorld = XMMatrixIdentity();
	object->matWorld = matScale;  //ワールド行列にスケーリングを反映
	object->matWorld = matRot;  //ワールド行列に回転を反映
	object->matWorld = matTrans;  //ワールド行列に平行移動を反映

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		//親オブジェクトのワールド行列を掛ける
		object->matWorld *= object->parent->matWorld;
	}
	//定数バッファへデータ転送
	ConstBufferData *constMap = nullptr;
	if (SUCCEEDED(object->constBuff->Map(0, nullptr, (void **)&constMap)))
	{
		constMap->color = XMFLOAT4(1, 1, 1, 1);  //RGBA
		constMap->mat = object->matWorld * matView * matProjection;
		object->constBuff->Unmap(0, nullptr);
	}
}

void DrawObject3d(Object3d *object, ComPtr<ID3D12GraphicsCommandList> cmdList,
	ComPtr<ID3D12DescriptorHeap> descHeap, D3D12_VERTEX_BUFFER_VIEW &vbView,
	D3D12_INDEX_BUFFER_VIEW &ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
	UINT numIndices)
{
	//頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap *ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, object->constBuff->GetGPUVirtualAddress());
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
	//描画コマンド
	cmdList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}



//パイプライン生成の関数
PipelineSet Object3dCreateGraphicsPipeline(ID3D12Device *dev)
{
	HRESULT result;

	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descTblRangeSRV;
	descTblRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);  //t0 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descTblRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicPS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{  //xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //法線ベクトル
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// 標準設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//デプスステンシルステートの設定
	//標準設定(深度テストを行う、書き込み許可、小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;  //深度値フォーマット

	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //標準設定

	blenddesc.BlendEnable = true;                 //ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を  0%使う

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;  //デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;   //1.0f - デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;  //使わない

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;   //ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;  //1.0f - ソースのアルファ値

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	PipelineSet pipelineSet;

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	//パイプラインとルートシグネチャを返す
	return pipelineSet;
}

PipelineSet spriteCreateGraphicsPipeline(ID3D12Device *dev)
{
	HRESULT result;

	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descTblRangeSRV;
	descTblRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);  //t0 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descTblRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{  //xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// 標準設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//デプスステンシルステートの設定
	//標準設定(深度テストを行う、書き込み許可、小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;  //深度値フォーマット

	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //標準設定

	blenddesc.BlendEnable = true;                 //ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を  0%使う

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;  //デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;   //1.0f - デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;  //使わない

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;   //ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;  //1.0f - ソースのアルファ値

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	PipelineSet pipelineSet;

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	//パイプラインとルートシグネチャを返す
	return pipelineSet;
}


Sprite SpriteCreate(ID3D12Device *dev, int window_width, int window_height, UINT texNumber, const SpriteCommon &
	spriteCommon, XMFLOAT2 anchorpoint,
	bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;
	Sprite sprite{};

	sprite.texNumber = texNumber;

	VertexPosUv vertices[] = {
		{{   0.0f, 100.0f, 0.0f}, { 0.0f , 1.0f} }, // 左下
		{{   0.0f,   0.0f, 0.0f}, { 0.0f , 0.0f} }, // 左上
		{{ 100.0f, 100.0f, 0.0f}, { 1.0f , 1.0f} }, // 右下
		{{ 100.0f,   0.0f, 0.0f}, { 1.0f , 0.0f} }, // 右上
	};

	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&sprite.vertBuff));

	VertexPosUv *vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);

	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sizeof(vertices);
	sprite.vbView.StrideInBytes = sizeof(vertices[0]);

	if (spriteCommon.texBuff[sprite.texNumber])
	{
		D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texNumber]->GetDesc();

		sprite.size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	//アンカーポイントをコピー
	sprite.anchorpoint = anchorpoint;

	sprite.isFlipX = isFlipX;
	sprite.isFlipY = isFlipY;

	SpriteTransferVertexBuffer(sprite, spriteCommon);

	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&sprite.constBuff));

	ConstBufferData *constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);

	constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}


void SpriteCommonBeginDraw(const SpriteCommon &spriteCommon, ID3D12GraphicsCommandList *cmdList)
{

	cmdList->SetPipelineState(spriteCommon.pipelineSet.pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(spriteCommon.pipelineSet.rootsignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap *ppHeaps[] = { spriteCommon.descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}


void SpriteDraw(const Sprite &sprite, ID3D12GraphicsCommandList *cmdList, const SpriteCommon &spriteCommon,
	ID3D12Device *dev)
{
	//非表示フラグがtrueなら
	if (sprite.isInvisible)
	{
		//描画せずに抜ける
		return;
	}

	cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);

	cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff->GetGPUVirtualAddress());

	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			spriteCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
			sprite.texNumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));


	cmdList->DrawInstanced(4, 1, 0, 0);
}


SpriteCommon SpriteCommonCreate(ID3D12Device *dev, int window_width, int window_height)
{
	HRESULT result = S_FALSE;

	SpriteCommon spriteCommon{};

	spriteCommon.pipelineSet = spriteCreateGraphicsPipeline(dev);

	spriteCommon.matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteCommon.descHeap));

	return spriteCommon;
}


void SpriteUpdate(Sprite &sprite, const SpriteCommon &spriteCommon)
{
	sprite.matWorld = XMMatrixIdentity();

	sprite.matWorld *= XMMatrixRotationZ(XMConvertToRadians(sprite.rotation));

	sprite.matWorld *= XMMatrixTranslation(sprite.position.x, sprite.position.y, sprite.position.z);

	ConstBufferData *constMap = nullptr;
	HRESULT result = sprite.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->mat = sprite.matWorld * spriteCommon.matProjection;
	constMap->color = sprite.color;
	sprite.constBuff->Unmap(0, nullptr);
}

void SpriteCommonLoadTexture(SpriteCommon &spriteCommon,
	UINT texnumber, const wchar_t *filename, ID3D12Device *dev)
{
	//異常な番号の指定を検出
	assert(texnumber <= spriteSRVCount - 1);

	HRESULT result;

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile
	(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	const Image *img = scratchImg.GetImage(0, 0, 0);  //生データ摘出

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テスクチャ用バッファの生成
	spriteCommon.texBuff[texnumber] = nullptr;
	result = dev->CreateCommittedResource
	(  //GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  //テスクチャ用指定
		nullptr,
		IID_PPV_ARGS(&spriteCommon.texBuff[texnumber])
	);

	//テクスチャバッファにデータ転送
	result = spriteCommon.texBuff[texnumber]->WriteToSubresource
	(
		0,
		nullptr,  //全領域へコピー
		img->pixels,  //元データアドレス
		(UINT)img->rowPitch,  //1ラインサイズ
		(UINT)img->slicePitch  //1枚サイズ
	);

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};  //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープのtexnumber番目にシェーダーリソースビュー作成
	dev->CreateShaderResourceView(spriteCommon.texBuff[texnumber].Get(),  //ビューと関連付けるバッファ
		&srvDesc,  //テクスチャの設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(spriteCommon.descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
}

void SpriteTransferVertexBuffer(const Sprite &sprite, const SpriteCommon &spriteCommon)
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] =
	{
		//  x      y      z       u     v
		{{}, {0.0f, 1.0f}}, // 左下
		{{}, {0.0f, 0.0f}}, // 左上
		{{}, {1.0f, 1.0f}}, // 右下
		{{}, {1.0f, 0.0f}}, // 右上
	};

	enum { LB, LT, RB, RT };

	float left = (0.0f - sprite.anchorpoint.x) * sprite.size.x;
	float right = (1.0f - sprite.anchorpoint.x) * sprite.size.x;
	float top = (0.0f - sprite.anchorpoint.y) * sprite.size.y;
	float bottom = (1.0f - sprite.anchorpoint.y) * sprite.size.y;

	if (sprite.isFlipX)
	{
		left = -left;
		right = -right;
	}

	if (sprite.isFlipY)
	{
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,  bottom, 0.0f }; // 左下
	vertices[LT].pos = { left,  top,    0.0f }; // 左上
	vertices[RB].pos = { right, bottom, 0.0f }; // 右下
	vertices[RT].pos = { right, top,    0.0f }; // 右上

	if (spriteCommon.texBuff[sprite.texNumber])
	{
		D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texNumber]->GetDesc();

		float tex_left = sprite.texBase.x / resDesc.Width;
		float tex_right = (sprite.texBase.x + sprite.texSize.x) / resDesc.Width;
		float tex_top = sprite.texBase.y / resDesc.Height;
		float tex_bottom = (sprite.texBase.y + sprite.texSize.y) / resDesc.Height;

		vertices[LB].uv = { tex_left,  tex_bottom }; // 左下
		vertices[LT].uv = { tex_left,  tex_top }; // 左上
		vertices[RB].uv = { tex_right, tex_bottom }; // 右下
		vertices[RT].uv = { tex_right, tex_top }; // 右上
	}

	VertexPosUv *vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);
}

void DebugText::Initialize(ID3D12Device *dev, int window_width, int window_height, UINT texNumber,
	SpriteCommon &spriteCommon)
{
	for (int i = 0; i < _countof(sprites); i++)
	{
		sprites[i] = SpriteCreate(dev, window_width, window_height, texNumber, spriteCommon, { 0, 0 }, false, false);
	}
}

void DebugText::Print(const SpriteCommon &spriteCommon, const std::string &text, float x, float y, float scale)
{
	//全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数超過
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		//1文字取り出す(ASCIIコードでしか成り立たない)
		const unsigned char &character = text[i];

		//ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex].position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex].texBase = { (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight };
		sprites[spriteIndex].texSize = { fontWidth, fontHeight };
		sprites[spriteIndex].size = { fontWidth * scale, fontHeight * scale };
		//頂点バッファ転送
		SpriteTransferVertexBuffer(sprites[spriteIndex], spriteCommon);
		//更新
		SpriteUpdate(sprites[spriteIndex], spriteCommon);
		//文字を1つ進める
		spriteIndex++;
	}
}

void DebugText::DrawAll(ID3D12GraphicsCommandList *cmdList, const SpriteCommon &spriteCommon, ID3D12Device *dev)
{
	//全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		SpriteDraw(sprites[i], cmdList, spriteCommon, dev);
	}

	spriteIndex = 0;
}

SoundData SoundLoadWave(const char *filename)
{
	HRESULT result;

	std::ifstream file;

	file.open(filename, std::ios_base::binary);

	assert(file.is_open());


	RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}

	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}


	FormatChunk format = {};

	file.read((char *)&format, sizeof(ChunkHeader));


	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);


	ChunkHeader data;
	file.read((char *)&data, sizeof(data));

	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		file.seekg(data.size, std::ios_base::cur);

		file.read((char *)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void SoundUnload(SoundData *soundData)
{
	//バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void SoundPlayWave(IXAudio2 *xAudio2, const SoundData &soundData)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}