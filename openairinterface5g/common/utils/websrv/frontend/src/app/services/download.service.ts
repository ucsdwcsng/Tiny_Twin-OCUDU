/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 * \utility to download a file from backend
 */
import {HttpClient} from "@angular/common/http";
import {HttpHeaders} from "@angular/common/http";
import {HttpParams} from "@angular/common/http";
import {Injectable} from "@angular/core";
import {route} from "src/commondefs";
import {environment} from "src/environments/environment";

@Injectable({
  providedIn : "root",
})

export class DownloadService {
  constructor(private http: HttpClient)
  {
  }

  getFile(url: string)
  {
    const token = "my JWT";
    const headers = new HttpHeaders().set("authorization", "Bearer " + token);
    const postparams = new HttpParams().set("fname", url);
    this.http.post(environment.backend + route + "/file", postparams, {headers, responseType : "blob" as "json"}).subscribe((response: any) => {
      let dataType = response.type;
      let binaryData = [];
      binaryData.push(response);
      let downloadLink = document.createElement("a");
      downloadLink.href = window.URL.createObjectURL(new Blob(binaryData, {type : dataType}));
      downloadLink.setAttribute("download", url);
      document.body.appendChild(downloadLink);
      downloadLink.click();
    })
  }
};
