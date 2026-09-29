/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 * \definitions of constants, enums and interfaces common to the whole frontend  
 */
export enum IArgType {
  boolean = "boolean",
  list = "list",
  loglvl = "loglvl",
  range = "range",
  number = "number",
  string = "string",
  configfile = "configfile",
  simuTypes = "simuTypes",
}

export interface IInfo {
  name: string;
  value: string;
  type: IArgType;
  modifiable: boolean; // set command ?
}

export const route = "oaisoftmodem/";
