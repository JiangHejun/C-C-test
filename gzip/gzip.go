/*
 * @Description:
 * @Author: Hejun Jiang
 * @Date: 2021-02-23 10:07:42
 * @LastEditTime: 2021-02-23 17:31:21
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */
package main

import (
	"compress/gzip"
	"io"
	"log"
	"os"
)

func Compress(fpath, zippath string) {
	outputFile, err := os.Create(zippath)
	if err != nil {
		log.Fatal(err)
	}
	gzipWriter := gzip.NewWriter(outputFile)
	defer gzipWriter.Close()

	file, err := os.Open(fpath)
	if err != nil {
		panic(err)
	}
	_, err = io.Copy(gzipWriter, file)
	if err != nil {
		log.Fatalf("%v\n", err)
	}
}

func UnCompress(fpath, unfpath string) {
	gzipFile, err := os.Open(fpath)
	if err != nil {
		log.Fatal(err)
	}
	defer gzipFile.Close()

	gzipReader, err := gzip.NewReader(gzipFile)
	if err == io.EOF {
		log.Fatalf("%v\n", err)
	}
	defer gzipReader.Close()

	outfileWriter, err := os.Create(unfpath)
	if err != nil {
		log.Fatal(err)
	}
	defer outfileWriter.Close()

	_, err = io.Copy(outfileWriter, gzipReader)
	if err != nil {
		log.Fatal(err)
	}
}

func main() {
	Compress("./5057.wav", "./ctest.gz")
	UnCompress("./ctest.gz", "./unzipped.wav")
}
