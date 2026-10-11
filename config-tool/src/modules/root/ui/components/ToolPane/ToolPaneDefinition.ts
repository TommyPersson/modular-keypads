export type ToolPaneDefinition = {
  id: string
  title: string
  // Shown below the icon in the tool pane selector, so it has to be a single short word
  shortTitle: string
  icon: any
  // Panes are separated by a divider in the tool pane selector where the group changes
  group?: "debug"
  component: any
}