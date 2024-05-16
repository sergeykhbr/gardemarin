/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <attribute.h>
#include "mainwindow.h"
#include <QApplication>

void printUsage() {
    printf("Qt-monitor for the Gardemarin board:\n"
           "\n"
           "    --cfg    Gardemarin json-config file. \n"
           "\n"
           "Example:\n"
           "    ./bin/qtmonitor --cfg ~/old/cfg.json"
           );
}

int main(int argc, char *argv[])
{
    AttributeType ConfigFileData;
    AttributeType JsonConfig;
    
    ConfigFileData.make_string("");
    if (argc == 1) {
        printUsage();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--cfg") == 0) {
            const char *cfgname = argv[++i];
            attr_read_json_file(cfgname, &ConfigFileData);
            if (ConfigFileData.size() == 0) {
                printf("Error: config file %s not found\n", cfgname);
                return 1;
            }
        }
    }

    JsonConfig.from_config(ConfigFileData.to_string());
    if (!JsonConfig.is_dict()) {
        printf("error: wrong configuration file format\n");
        printUsage();
        return 1;
    }
    

    QApplication a(argc, argv);
    MainWindow w(&JsonConfig);
    w.show();
    return a.exec();
}