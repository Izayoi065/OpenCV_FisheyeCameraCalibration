// OpenCV_FisheyeCameraCalibration.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#include "pch.h"

int main() {
	/* 探索するチェスボードの交点数 */
	const int horizontalCrossCount = 10;
	const int verticalCrossCount = 7;

	std::string inputstr;

	std::cout << "入力する動画ファイル名：";
	std::cin >> inputstr;
	std::string inputFilePass = "resource/Movies/CameraCaribration/" + inputstr;

	// 入力となる動画ファイルを入力
	cv::VideoCapture video(inputFilePass);
	if (!video.isOpened()) {
		// 入力ファイルの読み込みに失敗した場合
		std::cout << "入力ファイルの読み込みに失敗しました．" << std::endl;
		return -1;
	}

	/**
	* object_pointsにチェスボードのコーナー座標（三次元）を各画像ごとに入れる。
	* 通常は世界座標系の原点をチェスボード上のどこかに固定してZ軸をボードの法線方向にするため、どの画像にも同一のコーナー座標群が入る。
	*/
	std::vector<std::vector<cv::Point3f>> object_points;
	/**
	* findChessboardCorners()で検出した画像上のコーナー座標（サブピクセル精度）を入れる。
	* コーナーの並び順はobject_pointsと対応している必要
	*/
	std::vector<std::vector<cv::Point2f>> image_points;
	/**
	*
	*/
	std::vector<cv::Point3f> obj;
	for (int j = 0; j < horizontalCrossCount * verticalCrossCount; j++) {
		obj.push_back(cv::Point3f(j / horizontalCrossCount, j%horizontalCrossCount, 0.0f));
	}

	/* 入力ファイルのプロパティ */
	const int Width = video.get(CV_CAP_PROP_FRAME_WIDTH);		// 幅
	const int Heignt  = video.get(CV_CAP_PROP_FRAME_HEIGHT);	// 高さ
	const int frameCount = video.get(CV_CAP_PROP_FRAME_COUNT);	// 総フレーム数

	/* 入力ファイルの総フレーム数だけ探索を繰り返す */
	for (unsigned int i = 0; i < frameCount; i++) {
		cv::Mat frame, grayImage;
		video >> frame;	// 入力ファイルからフレーム画像を取得
		
		cv::cvtColor(frame, grayImage, CV_BGR2GRAY);	// グレイスケールへの変換

		/* 10-7チェスボードパターンを探索する */
		cv::Size chessboardPatterns(horizontalCrossCount, verticalCrossCount);
		std::vector<cv::Point2f> centers;
		bool found = findChessboardCorners(grayImage, chessboardPatterns, centers, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		if (found) {
			std::cout << "found chessboardPatterns : True" << std::endl;
			// 見つけたpointを高精度化する
			//cornerSubPix(gray, centers, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
			object_points.push_back(obj);
			image_points.push_back(centers);

			cv::FileStorage fi("projects/point.yml", cv::FileStorage::WRITE);
			fi << "centers" << centers;
			fi.release();

			// draw
			drawChessboardCorners(grayImage, chessboardPatterns, cv::Mat(centers), true);
			//cv::imshow("Flea3 Camera image(BGR)", grayImage);
		}
		else {
			std::cout << "found chessboardPatterns : False" << std::endl;
			//cv::imshow("Flea3 Camera image(BGR)", grayImage);
		}
	}
	std::cout << "入力ファイルの探索が終了しました．" << std::endl;

	/* チェスボードを撮影した画像サイズ */
	cv::Size img_Size(Width, Heignt);

	/* 魚眼カメラキャリブレーション */
	//Mat camera_matrix = Mat(Size(3, 3), CV_8U);
	//Mat distortion = Mat(Size(4, 1), CV_8U);
	cv::Mat camera_matrix = cv::Mat(cv::Size(3, 3), CV_32F);
	cv::Mat distortion = cv::Mat(cv::Size(4, 1), CV_32F);
	std::cout << "歪み補正の計算をしています..." << std::endl;
	double rms_err = cv::fisheye::calibrate(object_points, image_points, img_Size, camera_matrix, distortion,
		cv::noArray(), cv::noArray(), cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC | cv::fisheye::CALIB_CHECK_COND | cv::fisheye::CALIB_FIX_SKEW);

	// mtx,distパラメータをファイルに書き出し
	cv::FileStorage fs("projects/calibration.yml", cv::FileStorage::WRITE);
	fs << "camera_matrix" << camera_matrix;
	fs << "distortion" << distortion;
	fs.release();

	std::cout << "camera_matrix:" << std::endl << camera_matrix << std::endl;
	std::cout << "distortion:" << std::endl << distortion << std::endl;





	return 0;
}