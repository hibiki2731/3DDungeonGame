#pragma once
#include <string>
#include <vector>

//性能計測用のユーティリティ
/*
起動引数
  --bench        : ベンチモード(シナリオを自動実行し、終了時にCSVを出力して終了する)
  --stage=N      : リソース確保方式の段階 (1/2/3, 既定は3=現行)
                   1: オブジェクトごとに頂点バッファ、コンスタントバッファ、ディスクリプタヒープ、テクスチャを作成
                   2: 頂点バッファとテクスチャはキャッシュ、コンスタントバッファとヒープはオブジェクトごとに作成
                   3: 一括確保したコンスタントバッファ、ヒープをアロケータで割り当て(現行)
  --out=PATH     : CSVの出力先 (既定 bench_result.csv)
  --run=N        : 実行番号(CSVに記録するだけ)
  --seed=N       : 乱数シード (既定 12345, ベンチモード時のみ有効)
*/
namespace Bench {

	struct Config {
		bool enabled = false;
		int stage = 3;
		int run = 0;
		unsigned seed = 12345;
		std::string outPath = "bench_result.csv";
	};

	Config& config();
	void parseCommandLine();
	inline int stage() { return config().stage; }

	//高分解能タイマー(QueryPerformanceCounter)の現在時刻[ms]
	double nowMs();
	//WinMain開始時刻を記録する / そこからの経過時間[ms]
	void markStart();
	double msSinceStart();
	//プロセス生成からの経過時間[ms]
	double msSinceProcessCreation();

	//計測値の記録 metric:指標名 label:条件(N=100など) value:値
	void record(const std::string& metric, const std::string& label, double value);
	//CSVへ書き出す
	void writeCsv();

	//プロセスのメモリ使用量 [byte]
	struct ProcessMemory {
		size_t workingSet = 0;
		size_t privateBytes = 0;
	};
	ProcessMemory queryProcessMemory();

	//スコープを抜けるまでの時間を記録するRAIIタイマー
	class ScopeTimer {
	public:
		ScopeTimer(std::string metric, std::string label = "");
		~ScopeTimer();
		ScopeTimer(const ScopeTimer&) = delete;
		ScopeTimer& operator=(const ScopeTimer&) = delete;
	private:
		std::string mMetric;
		std::string mLabel;
		double mStart;
	};
}
