"""
Temas para botones y otros elementos estéticos.
"""


COLOR_PRIMARY = "#607D8B"
COLOR_PRIMARY_HOVER = "#455A64"
COLOR_DISABLED = "#BDBDBD"
COLOR_TEXT_DISABLED = "#757575"

COLOR_PROGRESS_FINISHED = "#4CAF50"
COLOR_PROGRESS_FINISHED2 = "#2ecc71"
COLOR_PROGRESS = "#2E7D32"

COLOR_WINDOW = "#EEEEEE"  # El color exacto de tu imagen


BUTTON_CALIB_STYLE = f"""
                        QPushButton {{
                            background-color: {COLOR_PRIMARY};
                            color: white;
                            padding: 10px;
                            border-radius: 4px;                        
                        }}
                        QPushButton:hover {{
                            background-color: {COLOR_PRIMARY_HOVER};
                        }}
                        QPushButton:disabled {{
                            background-color: {COLOR_DISABLED};
                            color: {COLOR_TEXT_DISABLED};
                        }}
                    """

BUTTON_CONNECT_STYLE =  f"""
                            QPushButton {{
                                background-color: {COLOR_PRIMARY};
                                color: white;
                                padding: 5px;
                                border-radius: 4px;
                            }}
                            QPushButton:hover {{
                                background-color: {COLOR_PRIMARY_HOVER};
                            }}
                            QPushButton:disabled {{
                                background-color: {COLOR_DISABLED};
                                color: {COLOR_TEXT_DISABLED};
                            }}
                        """

PROGRESS_BAR_CHARGING_STYLE =   f"""
                                    QProgressBar {{
                                        border: 1px solid #999;
                                        border-radius: 5px;
                                        text-align: center;
                                        height: 25px;
                                        background-color: {COLOR_WINDOW};
                                    }}
                                    QProgressBar::chunk {{
                                        background-color: {COLOR_PROGRESS};
                                        width: 10px;
                                    }}
                                """

PROGRES_BAR_IDLE_SYLE = f"""
                            QProgressBar {{
                                border: 1px solid #BCBCBC;
                                border-radius: 4px;
                                text-align: center;
                                background-color: {COLOR_WINDOW};
                                color: {COLOR_TEXT_DISABLED};
                            }}
                            QProgressBar::chunk {{
                                background-color: transparent; 
                            }}
                        """

PROGRESS_BAR_SUCCESS_STYLE = f"""
                                QProgressBar {{
                                    border: 1px solid #3d3d3d;
                                    border-radius: 5px;
                                    text-align: center;
                                    background-color: {COLOR_WINDOW};
                                    color: black;
                                    font-weight: bold;
                                }}

                                QProgressBar::chunk {{
                                    background-color: {COLOR_PROGRESS_FINISHED};  /* Color verde éxito */
                                    width: 20px;
                                }}
                            """