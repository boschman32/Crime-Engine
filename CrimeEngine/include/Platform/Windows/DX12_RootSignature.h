#pragma once


namespace cr {

	class DX12_Device;

	class DX12_RootSignature {
	public:
		DX12_RootSignature(DX12_Device&);
		~DX12_RootSignature();

		inline Microsoft::WRL::ComPtr<ID3D12RootSignature> GetSiganture() const { return m_signature; }

		static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	private:

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_signature;
	};
}