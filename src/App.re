[@react.component]
let make = () => {
  <ReasonApollo.Provider client=Gql_Client.instance>
    <OnBeforeUnload.Provider />
    Route.(
      switch (ReasonReact.Router.useUrl()->urlToRoute) {
      | Home => <UI_Layout> <UI_Repos.Index /> </UI_Layout>
      | Auth_Github => <UI_Auth_Github />
      | Auth_Callback(token) => <UI_Auth_Callback token />
      | Auth_Failure => <UI_Auth_Failure />
      | Auth_Logout => <UI_Auth_Logout />
      | MyRepos_Index => <UI_Layout> <UI_MyRepos.Index /> </UI_Layout>
      | Repo_Files_Index(owner, name) =>
        <UI_Layout> <UI_Repo_Files.Index owner name /> </UI_Layout>
      | Repo_Files_New(owner, name) =>
        <UI_Layout> <UI_Repo_Files.New owner name /> </UI_Layout>
      | Repo_Files_Edit(owner, name, path) =>
        <UI_Layout> <UI_Repo_Files.Edit owner name path /> </UI_Layout>
      | Repo_Files_Delete(owner, name, path) =>
        <UI_Layout> <UI_Repo_Files.Delete owner name path /> </UI_Layout>
      | Repo_Files_Show(owner, name, path) =>
        <UI_Layout> <UI_Repo_Files.Show owner name path /> </UI_Layout>
      | PullRequests_Index(owner, name) =>
        <UI_Layout> <UI_PullRequests.Index owner name /> </UI_Layout>
      | PullRequests_Show(owner, name, number) =>
        <UI_Layout> <UI_PullRequests.Show owner name number /> </UI_Layout>
      | PullRequests_New(owner, name, baseRefName, headRefName) =>
        <UI_Layout>
          <UI_PullRequests.New owner name baseRefName headRefName />
        </UI_Layout>
      | NotFound => "nothing found" |> React.string
      }
    )
  </ReasonApollo.Provider>;
};